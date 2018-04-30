open TestRenderer;

open Easy_format;

let list = {...list, align_closing: true};

let label = {...label, space_after_label: false};

let field = {...label, space_after_label: true};

let makeProp = (key, value) =>
  Label((Atom(key ++ "=", atom), label), Atom(value, atom));

let makeField = (key, value) => Label((Atom(key ++ ":", atom), field), value);

let rec formatInstance = instance => {
  let tag = componentName(instance.component);
  switch instance.subtree {
  | [] =>
    List(
      ("<" ++ tag, "", "/>", list),
      [
        makeProp("id", string_of_int(instance.id)),
        makeProp("state", instance.state)
      ]
    )
  | sub =>
    List(
      ("<" ++ tag ++ ">", "", "</" ++ tag ++ ">", list),
      sub |> List.map(formatInstance)
    )
  };
};

let formatElement = instances =>
  List(("[", "", "]", list), instances |> List.map(formatInstance));

let formatSubTreeChangeReact =
  fun
  | `NoChange => Atom("`NoChange", atom)
  | `Nested => Atom("`Nested", atom)
  | `PrependElement(x) =>
    List(("`PrependElement(", ",", ")", list), [formatElement(x)])
  | `ReplaceElements(oldElems, newElems) =>
    List(
      ("`ReplaceElements(", ",", ")", list),
      [oldElems, newElems] |> List.map(formatElement)
    );

let formatSubTreeChange =
  fun
  | `ContentChanged(change) =>
    List(
      ("`ContentChanged(", "", ")", list),
      [formatSubTreeChangeReact(change)]
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
        ("newInstance", formatInstance(update.newInstance))
      ]
      |> List.map(((key, value)) => makeField(key, value))
    )
  | ChangeComponent(update) =>
    List(
      ("ChangeComponent {", ",", "}", list),
      [
        ("oldSubtree", formatElement(update.oldSubtree)),
        ("newSubtree", formatElement(update.newSubtree)),
        ("oldInstance", formatInstance(update.oldInstance)),
        ("newInstance", formatInstance(update.newInstance))
      ]
      |> List.map(((key, value)) => makeField(key, value))
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
        makeField("subTreeChanged", formatSubTreeChange(update.subtreeChange)),
        makeField("updateLog", formatUpdateLog(update.updateLog^))
      ]
    );

let printElement = (formatter, instances) =>
  Pretty.to_formatter(formatter, formatElement(instances));

let printUpdateLog = (formatter, updateLog) =>
  Pretty.to_formatter(formatter, formatUpdateLog(updateLog));

let printTopLevelUpdateLog = (formatter, topLevelUpdate) =>
  Pretty.to_formatter(formatter, formatTopLevelUpdateLog(topLevelUpdate));
