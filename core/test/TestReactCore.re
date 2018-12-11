open Brisk_core;

module Implementation = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement =
    | Text(string)
    | View
    | Group(list(hostView))
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

  let getInstance = id =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id));
    } else {
      None;
    };
  let memoizeInstance = (id, instance) => Hashtbl.add(map, id, instance);

  let hostViewFromGroup = views => {name: "Group", element: Group(views)};

  let isDirty = ref(false);
  let markAsDirty = () => isDirty := true;

  let beginChanges = () => mountLog := [BeginChanges, ...mountLog^];

  let commitChanges = () => mountLog := [CommitChanges, ...mountLog^];

  let rec mountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    switch (child.element) {
    | Group(views) =>
      List.iteri(
        (i, child) => mountChild(~parent, ~child, ~position=i),
        views,
      )
    | _ => mountLog := [MountChild(parent, child, position), ...mountLog^]
    };

  let rec unmountChild = (~parent: hostView, ~child: hostView) =>
    switch (child.element) {
    | Group(views) =>
      List.iter(child => unmountChild(~parent, ~child), views)
    | _ => mountLog := [UnmountChild(parent, child), ...mountLog^]
    };

  let remountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    mountLog := [RemountChild(parent, child, position), ...mountLog^];
};

include ReactCore_Internal.Make(Implementation);

module HostView = {
  let forceHostInstance =
    fun
    | IFlat(Instance({hostInstance})) => Lazy.force(hostInstance)
    | renderedElement =>
      Implementation.hostViewFromGroup(
        List.map(
          (Instance({hostInstance})) => Lazy.force(hostInstance),
          Render.flattenRenderedElement(renderedElement),
        ),
      );
  let mountRenderedElement = (rootView, renderedElement) => {
    Implementation.beginChanges();
    let child = forceHostInstance(renderedElement);
    Implementation.mountChild(~parent=rootView, ~child, ~position=0);
    Implementation.commitChanges();
  };

  type change = [ SubtreeChange.t | SubtreeChange.updateGroupChange];

  let rec applyTopLevelUpdate =
          (root, renderedElement, topLevelUpdate: change) => {
    Implementation.beginChanges();
    switch (topLevelUpdate) {
    | `NoChange => ()
    | `Nested => ignore(forceHostInstance(renderedElement))
    | `PrependElement(newChild) =>
      Implementation.mountChild(
        ~parent=root,
        ~child=forceHostInstance(newChild),
        ~position=0,
      )
    | `ReplaceElements(toUnmount, toMount) =>
      Implementation.unmountChild(
        ~parent=root,
        ~child=forceHostInstance(toUnmount),
      );
      Implementation.mountChild(
        ~parent=root,
        ~child=forceHostInstance(toMount),
        ~position=0,
      );
    | `UpdateGroup(l) =>
      List.iter(
        applyTopLevelUpdate(root, renderedElement),
        (l :> list([ SubtreeChange.t | SubtreeChange.updateGroupChange])),
      )
    | `TopLevelChange(x) =>
      applyTopLevelUpdate(
        root,
        renderedElement,
        (x :> [ SubtreeChange.t | SubtreeChange.updateGroupChange]),
      )
    | `Reordered(_, _) => ()
    | `ReorderedAndNestedChange(_, _) => ()
    };
    Implementation.commitChanges();
  };
};
