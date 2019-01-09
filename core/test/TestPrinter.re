open Easy_format;

let list = {...list, align_closing: true};

let label = {...label, space_after_label: false};

let field = {...label, space_after_label: true};

let makeProp = (key, value) =>
  Label((Atom(key ++ "=", atom), label), Atom(value, atom));

let makeField = (key, value) =>
  Label((Atom(key ++ ":", atom), field), value);

let formatMountLogItem =
  TestReactCore.Implementation.(
    fun
    | BeginChanges => Atom("BeginChanges", atom)
    | CommitChanges => Atom("CommitChanges", atom)
    | MountChild(root, child, position) =>
      List(
        ("MountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_node(root), atom)),
          makeField("child", Atom(show_node(child), atom)),
          makeField("position", Atom(string_of_int(position), atom)),
        ],
      )
    | UnmountChild(root, child) =>
      List(
        ("UnmountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_node(root), atom)),
          makeField("child", Atom(show_node(child), atom)),
        ],
      )
    | RemountChild(root, child, from, to_) =>
      List(
        ("RemountChild (", ",", ")", list),
        [
          makeField("root", Atom(show_node(root), atom)),
          makeField("child", Atom(show_node(child), atom)),
          makeField("from", Atom(string_of_int(from), atom)),
          makeField("to", Atom(string_of_int(to_), atom)),
        ],
      )
    | ChangeText(from_, to_) =>
      List(
        ("ChangeText (", ",", ")", list),
        [
          makeField("previous", Atom(from_, atom)),
          makeField("next", Atom(to_, atom)),
        ],
      )
  );

let formatMountLog = mountLog =>
  List(("[", ",", "]", list), mountLog |> List.map(formatMountLogItem));

let printMountLog = (formatter, mountLog) =>
  Pretty.to_formatter(formatter, formatMountLog(mountLog));
