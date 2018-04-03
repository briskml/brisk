open ReasonReact;

type opaqueComponent =
  | Component(componentSpec('a, 'b, 'c, 'd)): opaqueComponent
  | InstanceAndComponent(component('a, 'b, 'c), instance('a, 'b, 'c)): opaqueComponent;

type testInstance = {
  component: opaqueComponent,
  id: Key.t,
  subtree: t,
  state: string
}
and t = list(testInstance);

type testSubTreeChange =
  | NoChange
  | Nested
  | PrependElement(t)
  | ReplaceElements(t, t);

type testUpdate = {
  oldInstance: testInstance,
  newInstance: testInstance,
  componentChanged: bool,
  stateChanged: bool,
  subTreeChanged: testSubTreeChange
};

type testUpdateEntry =
  | TopLevelUpdate(testSubTreeChange)
  | UpdateInstance(testUpdate);

type testUpdateLog = list(testUpdateEntry);

let rec convertInstance =
  fun
  | Instance({component, id, instanceSubTree, iState} as instance) => {
      component: InstanceAndComponent(component, instance),
      id,
      subtree: convertElement(instanceSubTree),
      state: component.printState(iState)
    }
  | NativeInstance(_, {component, id, instanceSubTree, iState} as instance) => {
      component: InstanceAndComponent(component, instance),
      id,
      subtree: convertElement(instanceSubTree),
      state: component.printState(iState)
    }
and convertElement =
  fun
  | IFlat(instances) => List.map(convertInstance, instances)
  | INested(_, elements) => List.flatten(List.map(convertElement, elements));

let convertSubTreeChange =
  fun
  | ReasonReact.UpdateLog.NoChange => NoChange
  | Nested => Nested
  | PrependElement(x) => PrependElement(convertElement(x))
  | ReplaceElements(oldElem, newElem) =>
    ReplaceElements(convertElement(oldElem), convertElement(newElem));

let render = element => RenderedElement.render(element);

let update = (element, next) => RenderedElement.update(element, next);

let convertUpdateLog = (updateLog: ReasonReact.UpdateLog.t) => {
  let rec convertUpdateLog = updateLogRef =>
    switch updateLogRef {
    | [] => []
    | [ReasonReact.UpdateLog.TopLevelUpdate(subtreeChange), ...t] => [
        TopLevelUpdate(convertSubTreeChange(subtreeChange)),
        ...convertUpdateLog(t)
      ]
    | [
        UpdateInstance({
          ReasonReact.UpdateLog.oldId,
          newId,
          oldOpaqueInstance,
          newOpaqueInstance,
          componentChanged,
          stateChanged,
          subTreeChanged
        }),
        ...t
      ] => [
        UpdateInstance({
          oldInstance: {
            ...convertInstance(oldOpaqueInstance),
            id: oldId
          },
          newInstance: {
            ...convertInstance(newOpaqueInstance),
            id: newId
          },
          componentChanged,
          stateChanged,
          subTreeChanged: convertSubTreeChange(subTreeChanged)
        }),
        ...convertUpdateLog(t)
      ]
    };
  List.rev(convertUpdateLog(updateLog^));
};

let compareComponents = (left, right) =>
  switch (left, right) {
  | (Component(_), Component(_))
  | (InstanceAndComponent(_, _), InstanceAndComponent(_, _)) => assert false
  | (Component(justComponent), InstanceAndComponent(comp, instance)) =>
    comp.handedOffInstance := Some(instance);
    let result =
      switch justComponent.handedOffInstance^ {
      | Some(_) => true
      | None => false
      };
    comp.handedOffInstance := None;
    result;
  | (InstanceAndComponent(comp, instance), Component(justComponent)) =>
    comp.handedOffInstance := Some(instance);
    let result =
      switch justComponent.handedOffInstance^ {
      | Some(_) => true
      | None => false
      };
    comp.handedOffInstance := None;
    result;
  };

let rec compareElement = (left, right) =>
  switch (left, right) {
  | (le, re) =>
    if (List.length(le) != List.length(re)) {
      false;
    } else {
      List.fold_left(
        (&&),
        true,
        List.map(compareInstance, List.combine(le, re))
      );
    }
  }
and compareInstance = ((left, right)) =>
  left.id == right.id
  && left.state == right.state
  && compareComponents(left.component, right.component)
  && compareElement(left.subtree, right.subtree);

let compareSubtree =
  fun
  | (NoChange, NoChange)
  | (Nested, Nested) => true
  | (PrependElement(left), PrependElement(right)) =>
    compareElement(left, right)
  | (ReplaceElements(left1, left2), ReplaceElements(right1, right2)) =>
    compareElement(left1, right1) && compareElement(left2, right2)
  | (_, _) => false;

let printList = (indent, lst) => {
  let indent = String.make(indent, ' ');
  "[" ++ String.concat(",\n", List.map(s => s, lst)) ++ "\n" ++ indent ++ "]";
};

let rec compareUpdateLog = (left, right) =>
  switch (left, right) {
  | ([], []) => true
  | ([UpdateInstance(x), ...t1], [UpdateInstance(y), ...t2]) =>
    x.stateChanged === y.stateChanged
    && x.componentChanged === y.componentChanged
    && compareSubtree((x.subTreeChanged, y.subTreeChanged))
    && compareInstance((x.oldInstance, y.oldInstance))
    && compareInstance((x.newInstance, y.newInstance))
    && compareUpdateLog(t1, t2)
  | ([TopLevelUpdate(x), ...t1], [TopLevelUpdate(y), ...t2]) =>
    compareSubtree((x, y)) && compareUpdateLog(t1, t2)
  | ([TopLevelUpdate(_), ..._], [UpdateInstance(_), ..._])
  | ([UpdateInstance(_), ..._], [TopLevelUpdate(_), ..._]) => false
  | ([_, ..._], [])
  | ([], [_, ..._]) => false
  };

let componentName = component =>
  switch component {
  | InstanceAndComponent(component, _) => component.debugName
  | Component(component) => component.debugName
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
