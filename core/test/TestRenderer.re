open TestReactCore;

type opaqueComponent =
  | Component(componentSpec('a, 'b, 'c, 'd)): opaqueComponent
  | InstanceAndComponent(component('a, 'b, 'c), instance('a, 'b, 'c))
    : opaqueComponent;

let equal_opaqueComponent = (left, right) =>
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

[@deriving eq]
type testInstance = {
  component: opaqueComponent,
  id: Key.t,
  subtree: t,
  state: string,
}
[@deriving eq]
and t = list(testInstance);

[@deriving eq]
type testSubTreeChangeReact = [
  | `NoChange
  | `Nested
  | `Reordered
  | `PrependElement(t)
  | `ReplaceElements(t, t)
];

[@deriving eq]
type testSubTreeChange = [
  testSubTreeChangeReact
  | `ContentChanged(testSubTreeChangeReact)
];

[@deriving eq]
type testUpdate = {
  oldInstance: testInstance,
  newInstance: testInstance,
  stateChanged: bool,
  subTreeChanged: testSubTreeChange,
};

[@deriving eq]
type testChangeComponent = {
  oldInstance: testInstance,
  newInstance: testInstance,
  oldSubtree: t,
  newSubtree: t,
};

[@deriving eq]
type testUpdateEntry =
  | UpdateInstance(testUpdate)
  | ChangeComponent(testChangeComponent);

[@deriving eq]
type testUpdateLog = list(testUpdateEntry);

[@deriving eq]
type testTopLevelUpdateLog = {
  subtreeChange: testSubTreeChange,
  updateLog: testUpdateLog,
};

[@deriving eq]
type optionTestTopLevelUpdateLog = option(testTopLevelUpdateLog);

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

let convertSubTreeChangeReact = (x: UpdateLog.subtreeChangeReact) =>
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
      updateLog: convertUpdateLog(topLevelUpdate.RenderedElement.updateLog),
    })
  | None => None;
