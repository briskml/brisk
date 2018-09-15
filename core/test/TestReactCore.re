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

  [@deriving eq]
  type testMountEntry =
    | BeginChanges
    | CommitChanges
    | MountChild(hostView, hostView, int)
    | UnmountChild(hostView, hostView)
    | RemountChild(hostView, hostView, int);

  [@deriving eq]
  type testMountLog = list(testMountEntry);

  let map: Hashtbl.t(int, hostView) = Hashtbl.create(1000);

  let mountLog = ref([]);

  let getInstance = id =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id));
    } else {
      None;
    };
  let memoizeInstance = (id, instance) => Hashtbl.add(map, id, instance);

  let isDirty = ref(false);
  let markAsDirty = () => isDirty := true;

  let beginChanges = () => mountLog := [BeginChanges, ...mountLog^];

  let commitChanges = () => mountLog := [CommitChanges, ...mountLog^];

  let mountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    mountLog := [MountChild(parent, child, position), ...mountLog^];

  let unmountChild = (~parent: hostView, ~child: hostView) =>
    mountLog := [UnmountChild(parent, child), ...mountLog^];

  let remountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    mountLog := [RemountChild(parent, child, position), ...mountLog^];
};

include ReactCore_Internal.Make(Implementation);
