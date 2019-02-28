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
          () =>
            Lwt.wakeup_later(
              onFinish,
              Yojson.Basic.from_string(
                String.concat("", prevStrings |> List.rev),
              ),
            ),
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

let get = (query, parse) => {
  open Httpkit_client;
  let uri =
    Uri.add_query_param'(
      Uri.of_string("http://brisk-graphqlhub.herokuapp.com"),
      ("query", query),
    );
  let request = Request.create(`GET, uri);
  Httpkit_lwt_client.(
    Lwt.Infix.(
      (
        switch (Uri.scheme(uri)) {
        | Some("https") => Https.send(request)
        | _ => Http.send(request)
        }
      )
      >>= (
        fun
        | Ok((_, body)) =>
          readResponse(body)
          |> Lwt_result.map(parse)
          |> Lwt_result.map_err(_ => ())
        | Error(_) => Lwt_result.fail()
      )
    )
  );
};
