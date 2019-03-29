module Error = {
  type t = unit;
};

let readResponse = responseBody => {
  let (readerPromise, onFinish) = Lwt.wait();
  let rec readResponse = prevStrings => {
    Httpaf.Body.(
      schedule_read(
        responseBody,
        ~on_eof=
          () => {
            let string = String.concat("", prevStrings |> List.rev);
            switch (Yojson.Basic.from_string(string)) {
            | exception x => Lwt.wakeup_exn(onFinish, x)
            | json =>
              switch (Yojson.Basic.Util.member("data", json)) {
              | `Null => Lwt.wakeup_exn(onFinish, Invalid_argument(string))
              | data => Lwt.wakeup_later(onFinish, data)
              }
            };
          },
        ~on_read=
          (fragment, ~off, ~len) => {
            let nextFragmentString = Bytes.create(len);
            Lwt_bytes.blit_to_bytes(
              fragment,
              off,
              nextFragmentString,
              0,
              len,
            );
            readResponse([
              Bytes.to_string(nextFragmentString),
              ...prevStrings,
            ]);
          },
      )
    );
  };
  readResponse([]);
  readerPromise |> Lwt_result.catch;
};

type query('a) = {
  parse: Yojson.Basic.t => 'a,
  variables: Yojson.Basic.t,
  query: string,
};

let get = ({variables, query, parse}: query('a)) => {
  open Httpkit_client;
  let uri =
    Uri.add_query_params'(
      Uri.of_string("http://brisk-graphqlhub.herokuapp.com/graphql"),
      [("query", query), ("variables", Yojson.Basic.to_string(variables))],
    );
  let request = Request.create(`POST, uri);
  Httpkit_lwt_client.(
    Lwt_result.Infix.(
      Http.send(request)
      |> Lwt_result.map_err(ignore)
      >>= (
        ((_, body)) =>
          Lwt_result.bind(readResponse(body), json => {
            switch (parse(json)) {
            | exception exc => Lwt_result.fail(exc)
            | x => Lwt_result.return(x)
            }
          }) |> Lwt_result.map_err(ignore)
      )
    )
  );
};
