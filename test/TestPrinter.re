open TestRenderer;

let printList = (indent, lst) => {
  let indent = String.make(indent, ' ');
  "[" ++ String.concat(",\n", List.map(s => s, lst)) ++ "\n" ++ indent ++ "]";
};

let rec printTreeFormatter = () => Fmt.hvbox(Fmt.list(printInstance()))
and printInstance = () =>
  Fmt.hvbox((formatter, instance) =>
    switch instance.subtree {
    | [] =>
      Fmt.pf(
        formatter,
        "<%s id=%s%s/>",
        componentName(instance.component),
        string_of_int(instance.id),
        switch instance.state {
        | "" => ""
        | x => " state=\"" ++ x ++ "\""
        }
      )
    | sub =>
      Fmt.pf(
        formatter,
        "<%s id=%s%s>@ %a@ </%s>",
        componentName(instance.component),
        string_of_int(instance.id),
        switch instance.state {
        | "" => ""
        | x => " state=\"" ++ x ++ "\""
        },
        printTreeFormatter(),
        sub,
        componentName(instance.component)
      )
    }
  );

let printElement = formatter => Fmt.pf(formatter, "%a", printTreeFormatter());

let printSubTreeChange = formatter =>
  Fmt.pf(
    formatter,
    "%a",
    Fmt.hvbox((formatter, change) =>
      switch change {
      | NoChange => Fmt.pf(formatter, "%s", "NoChange")
      | Nested => Fmt.pf(formatter, "%s", "Nested")
      | PrependElement(x) =>
        Fmt.pf(formatter, "PrependElement: %a@,", printElement, x)
      | ReplaceElements(oldElems, newElems) =>
        Fmt.pf(
          formatter,
          "ReplaceElements: %a@, %a@,",
          printElement,
          oldElems,
          printElement,
          newElems
        )
      }
    )
  );

let printUpdateLog = formatter => {
  let rec pp = () => Fmt.brackets(Fmt.list(~sep=Fmt.comma, printUpdateLog()))
  and printUpdateLog = ((), formatter, entry) =>
    switch entry {
    | TopLevelUpdate(subtreeChange) =>
      Fmt.pf(
        formatter,
        "%s (@[<hov> %a @])",
        "TopLevelUpdate",
        printSubTreeChange,
        subtreeChange
      )
    | UpdateInstance(update) =>
      Fmt.pf(
        formatter,
        "%s {@[<hov>@,componentChanged: %s,@ stateChanged: %s,@ subTreeChanged: %a,@ oldInstance: %a,@ newInstance: %a @]}",
        "UpdateInstance",
        string_of_bool(update.componentChanged),
        string_of_bool(update.stateChanged),
        printSubTreeChange,
        update.subTreeChanged,
        printInstance(),
        update.oldInstance,
        printInstance(),
        update.newInstance
      )
    };
  Fmt.pf(formatter, "%a", pp());
};
