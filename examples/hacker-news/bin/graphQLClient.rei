module Error: {type t;};

let get: (string, Yojson.Basic.json => 'a) => Lwt_result.t('a, Error.t);
