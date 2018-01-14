open React_ios;

module Implementation = {
  let map = Hashtbl.create(1000);
  type hostView =
    | Text(string)
    | View;
  let getInstance = (id) =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id))
    } else {
      None
    };
};

module ReasonReactM = ReactCore.Make(Implementation);

module ReasonReact = {
  include ReasonReactM;
  module Text = {
    let make = (~title="ImABox", ~onClick as _=?, _children) => {
      name: "Dummy",
      setProps: (_) => (),
      children: listToElement([]),
      nativeKey: Key.none,
      style: Layout.defaultStyle,
      make: (id) => {
        let elem = Implementation.Text(title);
        Hashtbl.add(Implementation.map, id, elem);
        elem
      }
    };
    let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
      nativeElement(~key?, make(~title?, ()));
  };
  let stringToElement = (string) => <Text title=string />;
};


/***
 * The simplest component. Composes nothing!
 */
module Box = {
  let make = (~title="ImABox", ~onClick as _=?, _children) =>
    ReasonReact.{
      name: "Dummy",
      setProps: (_) => (),
      children: ReasonReact.listToElement([]),
      nativeKey: Key.none,
      style: Layout.defaultStyle,
      make: (id) => {
        let elem = Implementation.Text(title);
        Hashtbl.add(Implementation.map, id, elem);
        elem
      }
    };
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    ReasonReact.nativeElement(~key?, make(~title?, ()));
};

module Div = {
  let make = (children) =>
    ReasonReact.{
      name: "Div",
      setProps: (_) => (),
      children: listToElement(children),
      nativeKey: Key.none,
      style: Layout.defaultStyle,
      make: (id) => {
        let elem = Implementation.View;
        Hashtbl.add(Implementation.map, id, elem);
        elem
      }
    };
  let createElement = (~key=?, ~children, ()) =>
    ReasonReact.nativeElement(~key?, make(children));
};

module BoxWrapper = {
  let component = ReasonReact.statelessComponent("BoxWrapper");
  let make = (~title="ImABox", ~twoBoxes=false, ~onClick as _=?, _children) => {
    ...component,
    initialState: () => (),
    render: (_self) =>
      twoBoxes ?
        <Div> <Box title /> <Box title /> </Div> : <Div> <Box title /> </Div>
  };
  let make = (~title="ImABox", ~twoBoxes=false, ~onClick as _=?, _children) => {
    ...component,
    initialState: () => (),
    render: (_self) => twoBoxes ? <Div /> : <Div />
  };
};


/***
 * Box with dynamic keys.
 */
module BoxWithDynamicKeys = {
  let component =
    ReasonReact.statelessComponent(~useDynamicKey=true, "BoxWithDynamicKeys");
  let make = (~title="ImABox", _children) => {
    ...component,
    render: (_self) => <Box title />
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
      ReasonReact.listToElement(state)
    }
  };
  let createElement = (~rAction, ~children, ()) =>
    ReasonReact.element(make(~rAction, children));
};


/***
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
          reduce(() => (), ());
          reduce(() => (), ());
          {mostRecentLabel: label, numChanges: state.numChanges + 1}
        } :
        state,
    render: ({state: {numChanges, mostRecentLabel}}) => {
      let title = Printf.sprintf("[%d, \"%s\"]", numChanges, mostRecentLabel);
      <Div> <Box title /> </Div>
    }
  };
  let createElement = (~label, ~children, ()) =>
    ReasonReact.element(make(~label, ()));
};

module StatelessButton = {
  let component = ReasonReact.statelessComponent("StatelessButton");
  let make =
      (~initialClickCount as _="noclicks", ~test as _="default", _children) => {
    ...component,
    render: (_self) => <Div> <Box /> </Div>
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
      <Div>
        (ReasonReact.stringToElement(string_of_int(state.buttonWrapperState)))
        <StatelessButton
          initialClickCount=("wrapped:" ++ (wrappedText ++ ":wrapped"))
        />
        <StatelessButton
          initialClickCount=("wrapped:" ++ (wrappedText ++ ":wrapped"))
        />
      </Div>
  };
  let createElement = (~wrappedText=?, ~children, ()) =>
    ReasonReact.element(make(~wrappedText?, ()));
};

let buttonWrapperJsx = <ButtonWrapper wrappedText="TestButtonUpdated!!!" />;

module ButtonWrapperWrapper = {
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
};

module UpdateAlternateClicks = {
  type action =
    | Click;
  let component = ReasonReact.reducerComponent("UpdateAlternateClicks");
  let make = (~rAction, _children) => {
    ...component,
    initialState: () => 0,
    printState: (state) => string_of_int(state),
    reducer: (Click, state) => Update(state + 1),
    shouldUpdate: ({newSelf: {state}}) => state mod 2 === 0,
    render: ({state, act}) => {
      ReasonReact.RemoteAction.subscribe(~act, rAction);
      ReasonReact.stringToElement("Rendered state is " ++ string_of_int(state))
    }
  };
};

Alcotest.run("Tests", []);
