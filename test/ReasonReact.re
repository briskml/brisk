open R2n2;

module Implementation = {
  let map = Hashtbl.create(1000);
  type hostView =
    | Text(string)
    | View;
  let getInstance = id =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id));
    } else {
      None;
    };
  let memoizeInstance = (id, instance) => Hashtbl.add(map, id, instance);
};

include ReactCore_Internal.Make(Implementation);
