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
        (Instance({component, id, instanceSubTree, iState} as instance)) => {
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
