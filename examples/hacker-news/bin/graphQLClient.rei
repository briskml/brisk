module Error: {type t;};

let get: (string, Yojson.Basic.t => 'a) => Lwt_result.t('a, Error.t);
