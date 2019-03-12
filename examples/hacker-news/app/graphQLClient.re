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

let get = (query, ~variables, parse) => {
  open Httpkit_client;
  let uri =
    Uri.add_query_params'(
      Uri.of_string("http://brisk-graphqlhub.herokuapp.com/graphql"),
      [("query", query), ("variables", Yojson.Basic.to_string(variables))],
    );
  let request = Request.create(`POST, uri);
  Httpkit_lwt_client.(
    Lwt.Infix.(
      Http.send(request)
      >>= (
        fun
        | Ok((_, body)) => {
            readResponse(body)
            |> Lwt_result.map(x =>
                 switch (parse(x)) {
                 | exception x =>
                   Printexc.to_string(x) |> print_endline;
                   raise(x);
                 | x => x
                 }
               )
            |> Lwt_result.map_err(exc =>
                 Printexc.to_string(exc) |> print_endline
               );
          }
        | Error(_) => {
            Lwt_result.fail();
          }
      )
    )
  );
};
