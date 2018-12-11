/*
open TestRenderer;
*/
open Easy_format;

let list = {...list, align_closing: true};

let label = {...label, space_after_label: false};

let field = {...label, space_after_label: true};

let makeProp = (key, value) =>
  Label((Atom(key ++ "=", atom), label), Atom(value, atom));

let makeField = (key, value) =>
  Label((Atom(key ++ ":", atom), field), value);

  /*
let componentName = component =>
  switch (component) {
  | InstanceAndComponent(component, _) => component.debugName
  | Component(component) => component.debugName
  };

let rec formatInstance = instance => {
  let tag = componentName(instance.component);
  switch (instance.subtree) {
  | [] =>
    List(
      ("<" ++ tag, "", "/>", list),
      [
        makeProp("id", string_of_int(instance.id)),
        makeProp("state", instance.state),
      ],
    )
  | sub =>
    List(
      ("<" ++ tag ++ ">", "", "</" ++ tag ++ ">", list),
      sub |> List.map(formatInstance),
    )
  };
};

let formatElement = instances =>
  List(("[", "", "]", list), instances |> List.map(formatInstance));
*/

let formatMountLogItem =
  TestReactCore.Implementation.(
    fun
    | BeginChanges => Atom("BeginChanges", atom)
    | CommitChanges => Atom("CommitChanges", atom)
    | MountChild(root, child, position) =>
      List(
        ("MountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_hostView(root), atom)),
          makeField("child", Atom(show_hostView(child), atom)),
          makeField("position", Atom(string_of_int(position), atom)),
        ],
      )
    | UnmountChild(root, child) =>
      List(
        ("UnmountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_hostView(root), atom)),
          makeField("child", Atom(show_hostView(child), atom)),
        ],
      )
    | RemountChild(root, child, position) =>
      List(
        ("RemountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_hostView(root), atom)),
          makeField("child", Atom(show_hostView(child), atom)),
          makeField("position", Atom(string_of_int(position), atom)),
        ],
      )
  );

let formatMountLog = mountLog =>
  List(("[", ",", "]", list), mountLog |> List.map(formatMountLogItem));

  /*
let formatSubTreeChangeReact =
  fun
  | `Reordered => Atom("`Reordered", atom)
  | `NoChange => Atom("`NoChange", atom)
  | `Nested => Atom("`Nested", atom)
  | `PrependElement(x) =>
    List(("`PrependElement(", ",", ")", list), [formatElement(x)])
  | `ReplaceElements(oldElems, newElems) =>
    List(
      ("`ReplaceElements(", ",", ")", list),
      [oldElems, newElems] |> List.map(formatElement),
    );

let formatSubTreeChange =
  fun
  | `ContentChanged(change) =>
    List(
      ("`ContentChanged(", "", ")", list),
      [formatSubTreeChangeReact(change)],
    )
  | #testSubTreeChangeReact as change => formatSubTreeChangeReact(change);

let formatUpdateLogItem =
  fun
  | UpdateInstance(update) =>
    List(
      ("UpdateInstance {", ",", "}", list),
      [
        ("stateChanged", Atom(string_of_bool(update.stateChanged), atom)),
        ("subTreeChanged", formatSubTreeChange(update.subTreeChanged)),
        ("oldInstance", formatInstance(update.oldInstance)),
        ("newInstance", formatInstance(update.newInstance)),
      ]
      |> List.map(((key, value)) => makeField(key, value)),
    )
  | ChangeComponent(update) =>
    List(
      ("ChangeComponent {", ",", "}", list),
      [
        ("oldSubtree", formatElement(update.oldSubtree)),
        ("newSubtree", formatElement(update.newSubtree)),
        ("oldInstance", formatInstance(update.oldInstance)),
        ("newInstance", formatInstance(update.newInstance)),
      ]
      |> List.map(((key, value)) => makeField(key, value)),
    );

let formatUpdateLog = updateLog =>
  List(("[", ",", "]", list), updateLog |> List.map(formatUpdateLogItem));

let formatTopLevelUpdateLog =
  fun
  | None => Atom("__none__", atom)
  | Some(update) =>
    List(
      ("TopLevelUpdate {", ",", "}", list),
      [
        makeField("subTreeChange", formatSubTreeChange(update.subtreeChange)),
        makeField("updateLog", formatUpdateLog(update.updateLog)),
      ],
    );

let printElement = (formatter, instances) =>
  Pretty.to_formatter(formatter, formatElement(instances));
  */

let printMountLog = (formatter, mountLog) =>
  Pretty.to_formatter(formatter, formatMountLog(mountLog));

  /*
let printUpdateLog = (formatter, updateLog) =>
  Pretty.to_formatter(formatter, formatUpdateLog(updateLog));

let printTopLevelUpdateLog = (formatter, topLevelUpdate) =>
  Pretty.to_formatter(formatter, formatTopLevelUpdateLog(topLevelUpdate));
*/
