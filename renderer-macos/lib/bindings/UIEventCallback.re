type callback = unit => unit;

let hooks = ref([]);

let addEventHandler = hook => hooks := [hook, ...hooks^];

let apply = f => f();

let make = (callback, ()) => {
  callback();
  List.iter(apply, hooks^);
  LwtFakeIOEvent.send();
};
