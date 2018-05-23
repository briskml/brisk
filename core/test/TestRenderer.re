open TestReactCore;

type opaqueComponent =
  | Component(componentSpec('a, 'b, 'c, 'd)): opaqueComponent
  | InstanceAndComponent(component('a, 'b, 'c), instance('a, 'b, 'c)): opaqueComponent;

type testInstance = {
  component: opaqueComponent,
  id: Key.t,
  subtree: t,
  state: string,
}
and t = list(testInstance);

type testSubTreeChangeReact = [
  | `NoChange
  | `Nested
  | `Reordered
  | `PrependElement(t)
  | `ReplaceElements(t, t)
];

type testSubTreeChange = [
  testSubTreeChangeReact
  | `ContentChanged(testSubTreeChangeReact)
];

type testUpdate = {
  oldInstance: testInstance,
  newInstance: testInstance,
  stateChanged: bool,
  subTreeChanged: testSubTreeChange,
};

type testChangeComponent = {
  oldInstance: testInstance,
  newInstance: testInstance,
  oldSubtree: t,
  newSubtree: t,
};

type testUpdateEntry =
  | UpdateInstance(testUpdate)
  | ChangeComponent(testChangeComponent);

type testUpdateLog = ref(list(testUpdateEntry));

type testTopLevelUpdateLog = {
  subtreeChange: testSubTreeChange,
  updateLog: testUpdateLog,
};

let rec convertInstance:
  'state 'action 'elementType .
  instance('state, 'action, 'elementType) => testInstance
 =
  ({component, id, instanceSubTree, iState} as instance) => {
    component: InstanceAndComponent(component, instance),
    id,
    subtree: convertElement(instanceSubTree),
    state: component.printState(iState),
  }
and convertElement =
  fun
  | IFlat(Instance(instance)) => [convertInstance(instance)]
  | INested(_, elements) =>
    List.flatten(List.map(convertElement, elements));

let rec convertSubTreeChangeReact = (x: UpdateLog.subtreeChangeReact) =>
  switch (x) {
  | `NoChange => `NoChange
  | `Nested => `Nested
  | `Reordered => `Reordered
  | `PrependElement(x) => `PrependElement(convertElement(x))
  | `ReplaceElements(oldElem, newElem) =>
    `ReplaceElements((convertElement(oldElem), convertElement(newElem)))
  };

let convertSubTreeChange =
  fun
  | `ContentChanged(x) => `ContentChanged(convertSubTreeChangeReact(x))
  | `NoChange as x
  | `Nested as x
  | `Reordered as x
  | `PrependElement(_) as x
  | `ReplaceElements(_, _) as x => convertSubTreeChangeReact(x);

let render = element => RenderedElement.render(element);

let convertUpdateLog = (updateLog: UpdateLog.t) => {
  let rec convertUpdateLog = (updateLogRef: list(UpdateLog.entry)) =>
    switch (updateLogRef) {
    | [] => []
    | [
        UpdateLog.UpdateInstance({
          oldInstance,
          newInstance,
          stateChanged,
          subTreeChanged,
        }),
        ...t,
      ] => [
        UpdateInstance({
          oldInstance: convertInstance(oldInstance),
          newInstance: convertInstance(newInstance),
          stateChanged,
          subTreeChanged: convertSubTreeChange(subTreeChanged),
        }),
        ...convertUpdateLog(t),
      ]
    | [
        UpdateLog.ChangeComponent({
          oldOpaqueInstance: Instance(oldInstance),
          newOpaqueInstance: Instance(newInstance),
        }),
        ...t,
      ] => [
        ChangeComponent({
          oldInstance: convertInstance(oldInstance),
          newInstance: convertInstance(newInstance),
          oldSubtree: convertElement(oldInstance.instanceSubTree),
          newSubtree: convertElement(newInstance.instanceSubTree),
        }),
        ...convertUpdateLog(t),
      ]
    };
  List.rev(convertUpdateLog(updateLog^));
};

let convertTopLevelUpdateLog:
  option(RenderedElement.topLevelUpdate) => option(testTopLevelUpdateLog) =
  fun
  | Some(topLevelUpdate) =>
    Some({
      subtreeChange: convertSubTreeChange(topLevelUpdate.subtreeChange),
      updateLog:
        ref(convertUpdateLog(topLevelUpdate.RenderedElement.updateLog)),
    })
  | None => None;

let compareComponents = (left, right) =>
  switch (left, right) {
  | (Component(_), Component(_))
  | (InstanceAndComponent(_, _), InstanceAndComponent(_, _)) =>
    assert(false)
  | (Component(justComponent), InstanceAndComponent(comp, instance)) =>
    comp.handedOffInstance := Some((instance, instance));
    let result =
      switch (justComponent.handedOffInstance^) {
      | Some(_) => true
      | None => false
      };
    comp.handedOffInstance := None;
    result;
  | (InstanceAndComponent(comp, instance), Component(justComponent)) =>
    comp.handedOffInstance := Some((instance, instance));
    let result =
      switch (justComponent.handedOffInstance^) {
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
        List.map(compareInstance, List.combine(le, re)),
      );
    }
  }
and compareInstance = ((left, right)) =>
  left.id == right.id
  && left.state == right.state
  && compareComponents(left.component, right.component)
  && compareElement(left.subtree, right.subtree);

let compareSubtreeReact =
  fun
  | (`NoChange, `NoChange)
  | (`Nested, `Nested)
  | (`Reordered, `Reordered) => true
  | (`PrependElement(left), `PrependElement(right)) =>
    compareElement(left, right)
  | (`ReplaceElements(left1, left2), `ReplaceElements(right1, right2)) =>
    compareElement(left1, right1) && compareElement(left2, right2)
  | (_, _) => false;

let compareSubtree =
  fun
  | (`ContentChanged(x), `ContentChanged(y)) => compareSubtreeReact((x, y))
  | (#testSubTreeChangeReact, #testSubTreeChangeReact) as x =>
    compareSubtreeReact(x)
  | (_, _) => false;

let rec compareUpdateLog = (left, right) =>
  switch (left, right) {
  | ([], []) => true
  | ([UpdateInstance(x), ...t1], [UpdateInstance(y), ...t2]) =>
    x.stateChanged === y.stateChanged
    && compareSubtree((x.subTreeChanged, y.subTreeChanged))
    && compareUpdateLog(t1, t2)
    && compareInstance((x.oldInstance, y.oldInstance))
    && compareInstance((x.newInstance, y.newInstance))
  | ([ChangeComponent(x), ...t1], [ChangeComponent(y), ...t2]) =>
    compareElement(x.oldSubtree, y.oldSubtree)
    && compareElement(x.newSubtree, y.newSubtree)
    && compareInstance((x.oldInstance, y.oldInstance))
    && compareInstance((x.newInstance, y.newInstance))
    && compareUpdateLog(t1, t2)
  | ([UpdateInstance(_), ..._], [_, ..._])
  | ([ChangeComponent(_), ..._], [_, ..._])
  | ([_, ..._], [])
  | ([], [_, ..._]) => false
  };

let compareTopLevelUpdateLog:
  (option(testTopLevelUpdateLog), option(testTopLevelUpdateLog)) => bool =
  (left, right) =>
    switch (left, right) {
    | (None, None) => true
    | (Some(x), Some(y)) =>
      compareSubtree((x.subtreeChange, y.subtreeChange))
      && compareUpdateLog(x.updateLog^, y.updateLog^)
    | (_, _) => false
    };

let componentName = component =>
  switch (component) {
  | InstanceAndComponent(component, _) => component.debugName
  | Component(component) => component.debugName
  };
