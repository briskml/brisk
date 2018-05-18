open Brisk;

module Implementation = {
  type hostView =
    | Text(string)
    | View;
  let map: Hashtbl.t(int, hostView) = Hashtbl.create(1000);
  let getInstance = id =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id));
    } else {
      None;
    };
  let memoizeInstance = (id, instance) => Hashtbl.add(map, id, instance);
};

include ReactCore_Internal.Make(Implementation);
