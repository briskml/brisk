open R2n2;

module ReasonReact = {
  module Implementation = {
    let map = Hashtbl.create(1000);
    type hostView =
      | Text(string)
      | View;
    let getInstance = id =>
      if (Hashtbl.mem(map, id)) {
        Some(Hashtbl.find(map, id));
      } else {
        None;
      };
  };
  include ReactCore_Internal.Make(Implementation);
  module Text = {
    /* FIXME: If a different prop is supplied as title, the change is not picked up by React. It's because make returns a host element and there's no way to know if a Host element is not changed. */
    let component = statefulNativeComponent("Text");
    let make = (~title="ImABox", _children) => {
      ...component,
      initialState: () => title,
      willReceiveProps: (_) => title,
      printState: (_) => title,
      render: (_) => {
        children: listToElement([]),
        make: id => {
          let elem = Implementation.Text(title);
          Hashtbl.add(Implementation.map, id, elem);
          elem;
        },
        updateInstance: (_) => ()
      }
    };
    let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
      element(~key?, make(~title?, ()));
  };
  let stringToElement = string => <Text title=string />;
};

module TestRenderer = {
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
  let printElement = formatter =>
    Fmt.pf(formatter, "%a", printTreeFormatter());
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
};

module Components = {
  /**
 * The simplest component. Composes nothing!
 */
  module Box = {
    open ReasonReact;
    let component = statefulNativeComponent("Box");
    let make = (~title="ImABox", ~onClick as _=?, _children) => {
      ...component,
      initialState: () => title,
      willReceiveProps: (_) => title,
      printState: (_) => title,
      render: (_) => {
        children: ReasonReact.listToElement([]),
        make: id => {
          let elem = Implementation.Text(title);
          Hashtbl.add(Implementation.map, id, elem);
          elem;
        },
        updateInstance: (_) => ()
      }
    };
    let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
      ReasonReact.element(~key?, make(~title?, ()));
  };
  module Div = {
    open ReasonReact;
    let component = statelessNativeComponent("Div");
    let make = children => {
      ...component,
      render: (_) => {
        children: listToElement(children),
        make: id => {
          let elem = Implementation.View;
          Hashtbl.add(Implementation.map, id, elem);
          elem;
        },
        updateInstance: (_) => ()
      }
    };
    let createElement = (~key=?, ~children, ()) =>
      ReasonReact.element(~key?, make(children));
  };
  module BoxWrapper = {
    let component = ReasonReact.statelessComponent("BoxWrapper");
    let make =
        (~title="ImABox", ~twoBoxes=false, ~onClick as _=?, _children)
        : ReasonReact.component(
            ReasonReact.stateless,
            unit,
            ReasonReact.reactElement
          ) => {
      ...component,
      initialState: () => (),
      render: _self =>
        twoBoxes ?
          <Div> <Box title /> <Box title /> </Div> : <Div> <Box title /> </Div>
    };
    let createElement = (~key=?, ~title=?, ~twoBoxes=?, ~children, ()) =>
      ReasonReact.element(~key?, make(~title?, ~twoBoxes?, ~onClick=(), ()));
  };
  /**
 * Box with dynamic keys.
 */
  module BoxWithDynamicKeys = {
    let component =
      ReasonReact.statelessComponent(
        ~useDynamicKey=true,
        "BoxWithDynamicKeys"
      );
    let make = (~title="ImABox", _children: list(ReasonReact.reactElement)) => {
      ...component,
      printState: (_) => title,
      render: _self => ReasonReact.listToElement([])
    };
    let createElement = (~title, ~children, ()) =>
      ReasonReact.element(make(~title, children));
  };
  module BoxList = {
    type action =
      | Create(string)
      | Reverse;
    let component = ReasonReact.reducerComponent("BoxList");
    let make = (~rAction, ~useDynamicKeys=false, _children) => {
      ...component,
      initialState: () => [],
      reducer: (action, state) =>
        switch action {
        | Create(title) =>
          ReasonReact.Update([
            useDynamicKeys ? <BoxWithDynamicKeys title /> : <Box title />,
            ...state
          ])
        | Reverse => ReasonReact.Update(List.rev(state))
        },
      render: ({state, act}) => {
        ReasonReact.RemoteAction.subscribe(~act, rAction);
        ReasonReact.listToElement(state);
      }
    };
    let createElement = (~rAction, ~useDynamicKeys=false, ~children, ()) =>
      ReasonReact.element(make(~rAction, ~useDynamicKeys, children));
  };
  /**
 * This component demonstrates several things:
 *
 * 1. Demonstration of making internal state hidden / abstract. Components
 * should encapsulate their state representation and should be free to change
 * it.
 *
 * 2. Demonstrates an equivalent of `componentWillReceiveProps`.
 * `componentWillReceiveProps` is like an "edge trigger" on props, and the
 * first item of the tuple shows how we implement that with this API.
 */
  module ChangeCounter = {
    type state = {
      numChanges: int,
      mostRecentLabel: string
    };
    let component = ReasonReact.reducerComponent("ChangeCounter");
    let make = (~label, _children) => {
      ...component,
      initialState: () => {mostRecentLabel: label, numChanges: 10},
      reducer: ((), state) =>
        ReasonReact.Update({...state, numChanges: state.numChanges + 1000}),
      willReceiveProps: ({state, reduce}) =>
        label != state.mostRecentLabel ?
          {
            print_endline("Will receive props");
            reduce(() => (), ());
            reduce(() => (), ());
            {mostRecentLabel: label, numChanges: state.numChanges + 1};
          } :
          state,
      render: ({state: {numChanges, mostRecentLabel}}) => ReasonReact.Flat([]),
      printState: ({numChanges, mostRecentLabel}) =>
        "[" ++ string_of_int(numChanges) ++ ", " ++ mostRecentLabel ++ "]"
    };
    let createElement = (~label, ~children, ()) =>
      ReasonReact.element(make(~label, ()));
  };
  module StatelessButton = {
    let component = ReasonReact.statelessComponent("StatelessButton");
    let make =
        (~initialClickCount as _="noclicks", ~test as _="default", _children) => {
      ...component,
      render: _self => <Div />
    };
    let createElement = (~initialClickCount=?, ~test=?, ~children, ()) =>
      ReasonReact.element(make(~initialClickCount?, ~test?, ()));
  };
  module ButtonWrapper = {
    type state = {buttonWrapperState: int};
    let component = ReasonReact.statefulComponent("ButtonWrapper");
    let make = (~wrappedText="default", _children) => {
      ...component,
      initialState: () => {buttonWrapperState: 0},
      render: ({state}) =>
        <StatelessButton
          initialClickCount=("wrapped:" ++ wrappedText ++ ":wrapped")
        />
    };
    let createElement = (~wrappedText=?, ~children, ()) =>
      ReasonReact.element(make(~wrappedText?, ()));
  };
  module ButtonWrapperWrapper = {
    let buttonWrapperJsx =
      <ButtonWrapper wrappedText="TestButtonUpdated!!!" />;
    let component = ReasonReact.statefulComponent("ButtonWrapperWrapper");
    let make = (~wrappedText="default", _children) => {
      ...component,
      initialState: () => "buttonWrapperWrapperState",
      render: ({state}) =>
        <Div>
          (ReasonReact.stringToElement(state))
          (ReasonReact.stringToElement("wrappedText:" ++ wrappedText))
          buttonWrapperJsx
        </Div>
    };
    let createElement = (~wrappedText=?, ~children, ()) =>
      ReasonReact.element(make(~wrappedText?, ()));
  };
  module UpdateAlternateClicks = {
    type action =
      | Click;
    let component = ReasonReact.reducerComponent("UpdateAlternateClicks");
    let make = (~rAction, _children) => {
      ...component,
      initialState: () => 0,
      printState: state => string_of_int(state),
      reducer: (Click, state) => Update(state + 1),
      shouldUpdate: ({newSelf: {state}}) => state mod 2 === 0,
      render: ({state, act}) => {
        ReasonReact.RemoteAction.subscribe(~act, rAction);
        ReasonReact.stringToElement(string_of_int(state));
      }
    };
    let createElement = (~rAction, ~children, ()) =>
      ReasonReact.element(make(~rAction, ()));
  };
};

let renderedElement =
  Alcotest.testable(
    (formatter, t) => TestRenderer.printElement(formatter, t),
    TestRenderer.compareElement
  );

let assertElement = (~label="", expected, rendered) =>
  Assert.check(
    renderedElement,
    label,
    expected,
    TestRenderer.convertElement(rendered)
  );

let updateLog =
  Alcotest.testable(
    (formatter, t) => TestRenderer.printUpdateLog(formatter, t),
    TestRenderer.compareUpdateLog
  );

let assertUpdateLog = (~label="", expected, actual) =>
  Assert.check(
    updateLog,
    label,
    expected,
    TestRenderer.convertUpdateLog(actual)
  );

let assertUpdate =
    (~label="", (expectedElement, expectedLog), (actualElement, actualLog)) => {
  assertElement(~label, expectedElement, actualElement);
  assertUpdateLog(~label, expectedLog, actualLog);
};

module TestComponents = {
  module Box = {
    let createElement = (~id, ~state="ImABox", ~children, ()) =>
      TestRenderer.{
        component: Component(Components.Box.component),
        id,
        state,
        subtree: children
      };
  };
  module Div = {
    let createElement = (~id, ~children, ()) =>
      TestRenderer.{
        component: Component(Components.Div.component),
        id,
        state: "",
        subtree: children
      };
  };
  module BoxWrapper = {
    let createElement = (~id, ~children, ()) =>
      TestRenderer.{
        component: Component(Components.BoxWrapper.component),
        id,
        state: "",
        subtree: children
      };
  };
  module ChangeCounter = {
    let createElement = (~id, ~label, ~counter, ~children: _, ()) =>
      TestRenderer.{
        component: Component(Components.ChangeCounter.component),
        id,
        state: Printf.sprintf("[%i, %s]", counter, label),
        subtree: []
      };
  };
  module ButtonWrapperWrapper = {
    open Components;
    let createElement = (~id, ~nestedText, ~children: _, ()) =>
      TestRenderer.{
        id,
        component: Component(ButtonWrapperWrapper.component),
        state: "",
        subtree: [
          {
            id: id + 1,
            component: Component(Div.component),
            state: "",
            subtree: [
              {
                id: id + 2,
                component: Component(ReasonReact.Text.component),
                state: "buttonWrapperWrapperState",
                subtree: []
              },
              {
                id: id + 3,
                component: Component(ReasonReact.Text.component),
                state: nestedText,
                subtree: []
              },
              {
                id: id + 4,
                component: Component(ButtonWrapper.component),
                state: "",
                subtree: [
                  {
                    id: id + 5,
                    component: Component(StatelessButton.component),
                    state: "",
                    subtree: [
                      {
                        component: Component(Div.component),
                        state: "",
                        id: id + 6,
                        subtree: []
                      }
                    ]
                  }
                ]
              }
            ]
          }
        ]
      };
  };
};

let suite =
  Components.[
    (
      "First Render",
      `Quick,
      () => {
        open TestRenderer;
        ReasonReact.GlobalState.reset();
        let component = BoxWrapper.make();
        let rendered = render(ReasonReact.element(component));
        let expected =
          TestComponents.[
            <BoxWrapper id=1>
              <Div id=2> <Box id=3 state="ImABox" /> </Div>
            </BoxWrapper>
          ];
        assertElement(expected, rendered);
      }
    ),
    (
      "Top level update",
      `Quick,
      () => {
        open TestRenderer;
        ReasonReact.GlobalState.reset();
        let actual =
          ReasonReact.RenderedElement.update(
            ReasonReact.RenderedElement.render(<BoxWrapper />),
            <BoxWrapper twoBoxes=true />
          );
        let twoBoxes =
          TestComponents.(
            <Div id=2>
              <Box id=4 state="ImABox" />
              <Box id=5 state="ImABox" />
            </Div>
          );
        let oneBox = TestComponents.(<Div id=2> <Box id=3 /> </Div>);
        let twoBoxesWrapper =
          TestComponents.(<BoxWrapper id=1> twoBoxes </BoxWrapper>);
        let expected = (
          [twoBoxesWrapper],
          [
            UpdateInstance({
              stateChanged: false,
              componentChanged: false,
              subTreeChanged:
                ReplaceElements(
                  [TestComponents.(<Box id=3 state="ImABox" />)],
                  TestComponents.[
                    <Box id=4 state="ImABox" />,
                    <Box id=5 state="ImABox" />
                  ]
                ),
              newInstance: twoBoxes,
              oldInstance: oneBox
            }),
            UpdateInstance({
              stateChanged: false,
              componentChanged: false,
              subTreeChanged: Nested,
              newInstance: twoBoxesWrapper,
              oldInstance:
                TestComponents.(<BoxWrapper id=1> oneBox </BoxWrapper>)
            }),
            TopLevelUpdate(Nested)
          ]
        );
        assertUpdate(expected, actual);
      }
    ),
    (
      "Change counter test",
      `Quick,
      () => {
        ReasonReact.GlobalState.reset();
        let rendered0 =
          ReasonReact.RenderedElement.render(
            <ChangeCounter label="defaultText" />
          );
        assertElement(
          [
            TestComponents.(
              <ChangeCounter id=1 label="defaultText" counter=10 />
            )
          ],
          rendered0
        );
        let (rendered1, _) as actual =
          TestRenderer.update(
            rendered0,
            <ChangeCounter label="defaultText" />
          );
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="defaultText" counter=10 />
              )
            ],
            []
          ),
          actual
        );
        let (rendered2, _) as actual =
          TestRenderer.update(
            rendered1,
            <ChangeCounter label="updatedText" />
          );
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="updatedText" counter=11 />
              )
            ],
            TestComponents.[
              UpdateInstance({
                componentChanged: false,
                stateChanged: true,
                subTreeChanged: NoChange,
                oldInstance:
                  <ChangeCounter id=1 label="defaultText" counter=10 />,
                newInstance:
                  <ChangeCounter id=1 label="updatedText" counter=11 />
              }),
              TopLevelUpdate(Nested)
            ]
          ),
          actual
        );
        let (rendered2f, _) as actual =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2);
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="updatedText" counter=2011 />
              )
            ],
            TestComponents.[
              UpdateInstance({
                componentChanged: false,
                stateChanged: true,
                subTreeChanged: NoChange,
                oldInstance:
                  <ChangeCounter id=1 label="updatedText" counter=11 />,
                newInstance:
                  <ChangeCounter id=1 label="updatedText" counter=2011 />
              })
            ]
          ),
          actual
        );
        let (rendered2f_mem, _) =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2f);
        Alcotest.(
          Assert.check(
            bool,
            "it is memoized",
            rendered2f_mem === rendered2f,
            true
          )
        );
        let (rendered2f_mem, _) =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2f_mem);
        Alcotest.(
          Assert.check(
            bool,
            "it is memoized",
            rendered2f_mem === rendered2f,
            true
          )
        );
        let (rendered3, _) =
          TestRenderer.update(
            rendered2f_mem,
            <ButtonWrapperWrapper wrappedText="updatedText" />
          );
        TestRenderer.convertElement(rendered3)
        |> Assert.check(
             renderedElement,
             "Switching Component Types from: ChangeCounter to ButtonWrapperWrapper",
             TestComponents.[
               <ButtonWrapperWrapper
                 id=2
                 nestedText="wrappedText:updatedText"
               />
             ]
           );
        let (rendered4, _) =
          TestRenderer.update(
            rendered3,
            <ButtonWrapperWrapper wrappedText="updatedTextmodified" />
          );
        TestRenderer.convertElement(rendered4)
        |> Assert.check(
             renderedElement,
             "Updating text in the button wrapper",
             TestComponents.[
               <ButtonWrapperWrapper
                 id=2
                 nestedText="wrappedText:updatedTextmodified"
               />
             ]
           );
        Assert.check(
          Alcotest.bool,
          "Memoized nested button wrapper",
          true,
          ReasonReact.(
            switch (rendered3, rendered4) {
            | (
                IFlat([
                  Instance({
                    instanceSubTree:
                      IFlat([
                        NativeInstance(
                          _,
                          {instanceSubTree: INested(_, [_, _, IFlat([x])])}
                        )
                      ])
                  })
                ]),
                IFlat([
                  Instance({
                    instanceSubTree:
                      IFlat([
                        NativeInstance(
                          _,
                          {instanceSubTree: INested(_, [_, _, IFlat([y])])}
                        )
                      ])
                  })
                ])
              ) =>
              x === y
            | _ => false
            }
          )
        );
      }
    ),
    (
      "Test Lists With Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let rAction = RemoteAction.create();
        let rendered0 =
          RenderedElement.render(<BoxList useDynamicKeys=true rAction />);
        RemoteAction.act(rAction, ~action=BoxList.Create("Hello"));
        let (rendered1, _) = RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) = RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) = RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial BoxList",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered2)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "World",
                     subtree: []
                   },
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered3)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   },
                   {
                     id: 3,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "World",
                     subtree: []
                   }
                 ]
               }
             ]
           );
      }
    ),
    (
      "Test Lists Without Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let rAction = RemoteAction.create();
        let rendered0 = RenderedElement.render(<BoxList rAction />);
        RemoteAction.act(rAction, ~action=BoxList.Create("Hello"));
        let (rendered1, _) = RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) = RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) = RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial BoxList",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered2)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(Box.component),
                     state: "World",
                     subtree: []
                   },
                   {
                     id: 4,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered3)
        |> Assert.check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   },
                   {
                     id: 4,
                     component: Component(Box.component),
                     state: "World",
                     subtree: []
                   }
                 ]
               }
             ]
           );
      }
    ),
    (
      "Deep Move Box With Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let box_ = <BoxWithDynamicKeys title="box to move" />;
        let rendered0 = RenderedElement.render(box_);
        let (rendered1, _) =
          RenderedElement.update(
            rendered0,
            Nested(
              "div",
              [ReasonReact.stringToElement("before"), Nested("div", [box_])]
            )
          );
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial Box",
             [
               {
                 id: 1,
                 component: Component(BoxWithDynamicKeys.component),
                 state: "box to move",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> Assert.check(
             renderedElement,
             "After update",
             [
               {
                 id: 2,
                 component: Component(Text.component),
                 state: "before",
                 subtree: []
               },
               {
                 id: 1,
                 component: Component(BoxWithDynamicKeys.component),
                 state: "box to move",
                 subtree: []
               }
             ]
           );
      }
    ),
    (
      "Test With Static Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let key1 = Key.create();
        let key2 = Key.create();
        let rendered0 =
          RenderedElement.render(
            ReasonReact.listToElement([
              <Box key=key1 title="Box1unchanged" />,
              <Box key=key2 title="Box2unchanged" />
            ])
          );
        let (rendered1, _) =
          RenderedElement.update(
            rendered0,
            ReasonReact.listToElement([
              <Box key=key2 title="Box2changed" />,
              <Box key=key1 title="Box1changed" />
            ])
          );
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial Boxes",
             [
               {
                 id: 1,
                 component: Component(Box.component),
                 state: "Box1unchanged",
                 subtree: []
               },
               {
                 id: 2,
                 component: Component(Box.component),
                 state: "Box2unchanged",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> Assert.check(
             renderedElement,
             "Swap Boxes",
             [
               {
                 id: 2,
                 component: Component(Box.component),
                 state: "Box2changed",
                 subtree: []
               },
               {
                 id: 1,
                 component: Component(Box.component),
                 state: "Box1changed",
                 subtree: []
               }
             ]
           );
      }
    ),
    (
      "Test Update on Alternate Clicks",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let result = (~state, ~text) => [
          {
            TestRenderer.id: 1,
            component: Component(UpdateAlternateClicks.component),
            state,
            subtree: [
              {
                id: 2,
                state: text,
                component: Component(Text.component),
                subtree: []
              }
            ]
          }
        ];
        let rAction = RemoteAction.create();
        let rendered =
          RenderedElement.render(<UpdateAlternateClicks rAction />);
        TestRenderer.convertElement(rendered)
        |> Assert.check(
             renderedElement,
             "Initial",
             result(~state="0", ~text="0")
           );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        Assert.check(
          renderedElement,
          "First click then flush",
          result(~state="1", ~text="0"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        Assert.check(
          renderedElement,
          "Second click then flush",
          result(~state="2", ~text="2"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        Assert.check(
          renderedElement,
          "Second click then flush",
          result(~state="3", ~text="2"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        Assert.check(
          renderedElement,
          "Second click then flush",
          result(~state="4", ~text="4"),
          TestRenderer.convertElement(rendered)
        );
      }
    )
  ];

Alcotest.run(~argv=[|"--verbose --color"|], "Tests", [("BoxWrapper", suite)]);
