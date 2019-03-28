module Error: {type t;};

type query('a) = {
  parse: Yojson.Basic.t => 'a,
  variables: Yojson.Basic.t,
  query: string,
};

let get: query('a) => Lwt_result.t('a, Error.t);
