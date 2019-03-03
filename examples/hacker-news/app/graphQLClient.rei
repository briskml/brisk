module Error: {type t;};

let get: (string, ~variables: Yojson.Basic.t, Yojson.Basic.t => 'a) => Lwt_result.t('a, Error.t);
