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

module HostView = {
  let forceHostInstance = x =>
    OutputTree.InstanceSubtree.toList(x) |> List.map(Lazy.force);
  let mountRenderedElement =
      ({nearestHostOutputNode, renderedElement}: RenderedElement.t) => {
    Implementation.beginChanges();
    forceHostInstance(renderedElement)
    |> List.fold_left(
         (parent, child) =>
           Implementation.mountChild(~parent, ~child, ~position=0),
         Lazy.force(nearestHostOutputNode),
       )
    |> ignore;
    Implementation.commitChanges();
  };

  type change = [ SubtreeChange.t | SubtreeChange.updateGroupChange];

  let rec applyTopLevelUpdate =
          (
            {renderedElement} as prevRenderedElement: RenderedElement.t,
            topLevelUpdate: change,
          ) => {
    Implementation.beginChanges();
    switch (topLevelUpdate) {
    | `NoChange => ()
    | `Nested => ignore(forceHostInstance(renderedElement))
    | `PrependElement(_newChild) => ()
    /*
     Implementation.mountChild(
       ~parent=root,
       ~child=forceHostInstance(newChild),
       ~position=0,
     )
     */
    | `ReplaceSubtree(_) => ()
    | `UpdateGroup(l) =>
      List.iter(
        applyTopLevelUpdate(prevRenderedElement),
        (l :> list([ SubtreeChange.t | SubtreeChange.updateGroupChange])),
      )
    | `TopLevelChange(x) =>
      applyTopLevelUpdate(
        prevRenderedElement,
        (x :> [ SubtreeChange.t | SubtreeChange.updateGroupChange]),
      )
    | `Reordered(_, _) => ()
    | `ReorderedAndNestedChange(_, _) => ()
    };
    Implementation.commitChanges();
  };
};
