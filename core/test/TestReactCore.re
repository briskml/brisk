open Brisk_core;

module Implementation = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement =
    | Text(string)
    | View
  [@deriving (show({with_path: false}), eq)]
  and hostView = {
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

  let isDirty = ref(false);
  let markAsDirty = () => isDirty := true;

  let beginChanges = () => mountLog := [BeginChanges, ...mountLog^];

  let commitChanges = () => mountLog := [CommitChanges, ...mountLog^];

  let mountChild = (~parent: hostView, ~child: hostView, ~position: int) => {
    switch (child.element) {
    | _ => mountLog := [MountChild(parent, child, position), ...mountLog^]
    };
    parent;
  };

  let unmountChild = (~parent: hostView, ~child: hostView) => {
    switch (child.element) {
    | _ => mountLog := [UnmountChild(parent, child), ...mountLog^]
    };
    parent;
  };

  let remountChild =
      (~parent: hostView, ~child: hostView, ~from as _: int, ~to_: int) => {
    mountLog := [RemountChild(parent, child, to_), ...mountLog^];
    parent;
  };
};

include ReactCore_Internal.Make(Implementation);
