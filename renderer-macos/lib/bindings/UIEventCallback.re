type callback = unit => unit;

let hooks = ref([]);

let addEventHandler = hook => hooks := [hook, ...hooks^];

let apply = f => f();

let make = (callback, ()) => {
  callback();
  /* This is a workaround for non-deterministic crashes 
   * when OCaml wants to allocate huge amounts of memory.
   * This crash still happens but is a lot less frequent.
   * A very negative side effect is terrible performance.
   */
  Gc.minor();
  List.iter(apply, hooks^);
  LwtFakeIOEvent.send();
};
