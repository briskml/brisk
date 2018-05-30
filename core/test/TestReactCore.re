open Brisk_core;

module Implementation = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement =
    | Text(string)
    | View;

  [@deriving (show({with_path: false}), eq)]
  type hostView = {
    name: string,
    element: hostElement,
  };

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
