open TestReactCore;

/**
 * The simplest component. Composes nothing!
 */
module Box = {
  let component = statefulNativeComponent("Box");
  let make = (~title="ImABox", ~onClick as _=?, _children) => {
    ...component,
    initialState: () => title,
    willReceiveProps: _ => title,
    printState: _ => title,
    render: _ => {
      children: listToElement([]),
      make: () => Implementation.{name: "Box", element: Text(title)},
      updateInstance: (_, _) => (),
      shouldReconfigureInstance: (~oldState, ~newState) =>
        oldState != newState,
    },
  };
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    element(~key?, make(~title?, ()));
};

module Div = {
  let component = statelessNativeComponent("Div");
  let make = children => {
    ...component,
    render: _ => {
      children: listToElement(children),
      make: () => Implementation.{name: "Div", element: View},
      updateInstance: (_, _) => (),
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => false,
    },
  };
  let createElement = (~key=?, ~children, ()) =>
    element(~key?, make(children));
};

module Text = {
  /**
   * FIXME: If a different prop is supplied as title, the change is not picked up by React.
   * It's because make returns a host element and there's no way to know if a Host element
   * is not changed.
   * */
  let component = statefulNativeComponent("Text");
  let shouldUpdate = (!=);
  let make = (~title="ImABox", _children) => {
    ...component,
    initialState: () => title,
    willReceiveProps: _ => title,
    shouldUpdate:
      ({oldSelf: {state: oldState}, newSelf: {state: newState}}) =>
      shouldUpdate(oldState, newState),
    printState: state => state,
    render: _ => {
      children: listToElement([]),
      make: () => Implementation.{name: "Text", element: Text(title)},
      updateInstance: (_, _) => (),
      shouldReconfigureInstance: (~oldState, ~newState) =>
        shouldUpdate(oldState, newState),
    },
  };
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    element(~key?, make(~title?, ()));
};

let stringToElement = string => <Text title=string />;

module BoxWrapper = {
  let component = statelessComponent("BoxWrapper");
  let make =
      (~title="ImABox", ~twoBoxes=false, ~onClick as _=?, _children)
      : component(stateless, unit, reactElement, syntheticOutputNode) => {
    ...component,
    initialState: () => (),
    render: _self =>
      twoBoxes ?
        <Div> <Box title /> <Box title /> </Div> : <Div> <Box title /> </Div>,
  };
  let createElement = (~key=?, ~title=?, ~twoBoxes=?, ~children as _, ()) =>
    element(~key?, make(~title?, ~twoBoxes?, ~onClick=(), ()));
};

/**
 * Box with dynamic keys.
 */
module BoxItemDynamic = {
  let component = statelessComponent(~useDynamicKey=true, "BoxItemDynamic");
  let make = (~title="ImABox", _children: list(reactElement)) => {
    ...component,
    printState: _ => title,
    render: _self => listToElement([]),
  };
  let createElement = (~title, ~children, ()) =>
    element(make(~title, children));
};

module BoxList = {
  type action =
    | Create(string)
    | Reverse;
  let component = reducerComponent("BoxList");
  let make = (~rAction, ~useDynamicKeys=false, _children) => {
    ...component,
    initialState: () => [],
    reducer: (action, state) =>
      switch (action) {
      | Create(title) =>
        Update([
          useDynamicKeys ? <BoxItemDynamic title /> : <Box title />,
          ...state,
        ])
      | Reverse => Update(List.rev(state))
      },
    render: ({state, act}) => {
      RemoteAction.subscribe(~act, rAction);
      listToElement(state);
    },
  };
  let createElement = (~rAction, ~useDynamicKeys=false, ~children, ()) =>
    element(make(~rAction, ~useDynamicKeys, children));
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
    mostRecentLabel: string,
  };
  let component = reducerComponent("ChangeCounter");
  let make = (~label, _children) => {
    ...component,
    initialState: () => {mostRecentLabel: label, numChanges: 0},
    reducer: ((), state) =>
      Update({...state, numChanges: state.numChanges + 10}),
    willReceiveProps: ({state, reduce}) =>
      label != state.mostRecentLabel ?
        {
          print_endline("Will receive props");
          reduce(() => (), ());
          reduce(() => (), ());
          {mostRecentLabel: label, numChanges: state.numChanges + 1};
        } :
        state,
    render: ({state: {numChanges: _, mostRecentLabel: _}}) =>
      Nested([]),
    printState: ({numChanges, mostRecentLabel}) =>
      "[" ++ string_of_int(numChanges) ++ ", " ++ mostRecentLabel ++ "]",
  };
  let createElement = (~label, ~children as _, ()) =>
    element(make(~label, ()));
};

module StatelessButton = {
  let component = statelessComponent("StatelessButton");
  let make =
      (~initialClickCount as _="noclicks", ~test as _="default", _children) => {
    ...component,
    render: _self => <Div />,
  };
  let createElement = (~initialClickCount=?, ~test=?, ~children as _, ()) =>
    element(make(~initialClickCount?, ~test?, ()));
};

module ButtonWrapper = {
  type state = {buttonWrapperState: int};
  let component = statelessComponent("ButtonWrapper");
  let make = (~wrappedText="default", _children) => {
    ...component,
    render: ({state: _}) =>
      <StatelessButton
        initialClickCount=("wrapped:" ++ wrappedText ++ ":wrapped")
      />,
  };
  let createElement = (~wrappedText=?, ~children as _, ()) =>
    element(make(~wrappedText?, ()));
};

module ButtonWrapperWrapper = {
  let buttonWrapperJsx = <ButtonWrapper wrappedText="TestButtonUpdated!!!" />;
  let component = statelessComponent("ButtonWrapperWrapper");
  let make = (~wrappedText="default", _children) => {
    ...component,
    render: _ => <Div> (stringToElement(wrappedText)) buttonWrapperJsx </Div>,
  };
  let createElement = (~wrappedText=?, ~children as _, ()) =>
    element(make(~wrappedText?, ()));
};

module UpdateAlternateClicks = {
  type action =
    | Click;
  let component = reducerComponent("UpdateAlternateClicks");
  let make = (~rAction, _children) => {
    ...component,
    initialState: () => 0,
    printState: state => string_of_int(state),
    reducer: (Click, state) => Update(state + 1),
    shouldUpdate: ({newSelf: {state}}) => state mod 2 === 0,
    render: ({state, act}) => {
      RemoteAction.subscribe(~act, rAction);
      stringToElement(string_of_int(state));
    },
  };
  let createElement = (~rAction, ~children as _, ()) =>
    element(make(~rAction, ()));
};

module ToggleClicks = {
  type action =
    | Click;
  let component = reducerComponent("ToggleClicks");
  let make = (~rAction, _children) => {
    ...component,
    initialState: () => false,
    printState: state => string_of_bool(state),
    reducer: (Click, state) => Update(!state),
    render: ({state, act}) => {
      RemoteAction.subscribe(~act, rAction);
      if (state) {
        <Div> <Text title="cell1" /> <Text title="cell2" /> </Div>;
      } else {
        <Div> <Text title="well" /> </Div>;
      };
    },
  };
  let createElement = (~rAction, ~children as _, ()) =>
    element(make(~rAction, ()));
};
