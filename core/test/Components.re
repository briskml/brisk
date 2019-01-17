open TestReactCore;

/**
 * The simplest component. Composes nothing!
 */
module Box = {
  let component = nativeComponent("Box");
  let make = (~title="ImABox", ~onClick as _=?, _children) =>
    component((_: Hooks.empty) =>
      {
        children: listToElement([]),
        make: () => Implementation.{name: "Box", element: Text(title)},
        configureInstance: (~isFirstRender, instance) =>
          isFirstRender ?
            instance : Implementation.{name: "Box", element: Text(title)},
      }
    );
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    element(~key?, make(~title?, ()));
};

module Div = {
  let component = nativeComponent("Div");
  let make = children =>
    component((_: Hooks.empty) =>
      {
        make: () => Implementation.{name: "Div", element: View},
        configureInstance: (~isFirstRender as _, d) => d,
        children: listToElement(children),
      }
    );
  let createElement = (~key=?, ~children, ()) =>
    element(~key?, make(children));
};

module Text = {
  type state = {
    current: string,
    prev: string,
  };
  let component = nativeComponent("Text");
  let make = (~title="ImABox", _children) =>
    component(slots => {
      let (prevTitle, setTitle, slots) = Hooks.ref(title, slots);
      let _slots: Hooks.empty =
        Hooks.effect(
          Always,
          () => {
            setTitle(title);
            None;
          },
          slots,
        );
      {
        make: () => Implementation.{name: "Text", element: Text(title)},
        configureInstance: (~isFirstRender, t) => {
          if (prevTitle != title || isFirstRender) {
            Implementation.mountLog :=
              [
                Implementation.ChangeText(prevTitle, title),
                ...Implementation.mountLog^,
              ];
          };
          t;
        },
        children: listToElement([]),
      };
    });
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    element(~key?, make(~title?, ()));
};

let stringToElement = string => <Text title=string />;

module BoxWrapper = {
  let component = component("BoxWrapper");
  let make = (~title="ImABox", ~twoBoxes=false, ~onClick as _=?, _children) =>
    component((_: Hooks.t(unit, unit)) =>
      twoBoxes ?
        <Div> <Box title /> <Box title /> </Div> : <Div> <Box title /> </Div>
    );
  let createElement = (~key=?, ~title=?, ~twoBoxes=?, ~children as _, ()) =>
    element(~key?, make(~title?, ~twoBoxes?, ~onClick=(), ()));
};

/**
 * Box with dynamic keys.
 */
module BoxItemDynamic = {
  let component = component(~useDynamicKey=true, "BoxItemDynamic");
  let make = (~title="ImABox", _children: list(syntheticElement)) =>
    component((_: Hooks.empty) => stringToElement(title));
  let createElement = (~title, ~children, ()) =>
    element(make(~title, children));
};

module BoxList = {
  type action =
    | Create(string)
    | Reverse;
  let component = component("BoxList");
  let make = (~rAction, ~useDynamicKeys=false, _children) =>
    component(hooks => {
      let (state, dispatch, hooks) =
        Hooks.reducer(
          ~initialState=[],
          (action, state) =>
            switch (action) {
            | Create(title) => [
                useDynamicKeys ? <BoxItemDynamic title /> : <Box title />,
                ...state,
              ]
            | Reverse => List.rev(state)
            },
          hooks,
        );
      let _: Hooks.empty =
        Hooks.effect(
          OnMount,
          () => Some(RemoteAction.subscribe(~handler=dispatch, rAction)),
          hooks,
        );

      listToElement(state);
    });
  let createElement = (~rAction, ~useDynamicKeys=false, ~children, ()) =>
    element(make(~rAction, ~useDynamicKeys, children));
};

module StatelessButton = {
  let component = component("StatelessButton");
  let make =
      (~initialClickCount as _="noclicks", ~test as _="default", _children) =>
    component((_: Hooks.empty) => <Div />);
  let createElement = (~initialClickCount=?, ~test=?, ~children as _, ()) =>
    element(make(~initialClickCount?, ~test?, ()));
};

module ButtonWrapper = {
  let component = component("ButtonWrapper");
  let make = (~wrappedText="default", _children) =>
    component((_: Hooks.empty) =>
      <StatelessButton
        initialClickCount={"wrapped:" ++ wrappedText ++ ":wrapped"}
      />
    );
  let createElement = (~wrappedText=?, ~children as _, ()) =>
    element(make(~wrappedText?, ()));
};

module ButtonWrapperWrapper = {
  let buttonWrapperJsx = <ButtonWrapper wrappedText="TestButtonUpdated!!!" />;
  let component = component("ButtonWrapperWrapper");
  let make = (~wrappedText="default", _children) =>
    component((_: Hooks.empty) =>
      <Div> {stringToElement(wrappedText)} buttonWrapperJsx </Div>
    );
  let createElement = (~wrappedText=?, ~children as _, ()) =>
    element(make(~wrappedText?, ()));
};

module UpdateAlternateClicks = {
  type action =
    | Click;
  let component = component("UpdateAlternateClicks");
  let make = (~rAction, _children) =>
    component(hooks => {
      let (state, dispatch, hooks) =
        Hooks.reducer(
          ~initialState=ref(0),
          (Click, state) =>
            /* FIXME: make this pure */
            state^ mod 2 === 0 ?
              {
                state := state^ + 1;
                state;
              } :
              ref(state^ + 1),
          hooks,
        );
      let _: Hooks.empty =
        Hooks.effect(
          OnMount,
          () => Some(RemoteAction.subscribe(~handler=dispatch, rAction)),
          hooks,
        );
      stringToElement(string_of_int(state^));
    });
  let createElement = (~rAction, ~children as _, ()) =>
    element(make(~rAction, ()));
};

module ToggleClicks = {
  type action =
    | Click;
  let component = component("ToggleClicks");
  let make = (~rAction, _children) =>
    component(hooks => {
      let (state, dispatch, hooks) =
        Hooks.reducer(~initialState=false, (Click, state) => !state, hooks);
      let _: Hooks.empty =
        Hooks.effect(
          OnMount,
          () => Some(RemoteAction.subscribe(~handler=dispatch, rAction)),
          hooks,
        );
      if (state) {
        <Div> <Text title="cell1" /> <Text title="cell2" /> </Div>;
      } else {
        <Div> <Text title="well" /> </Div>;
      };
    });
  let createElement = (~rAction, ~children as _, ()) =>
    element(make(~rAction, ()));
};

module EmptyComponent = {
  let component = component("Box");
  let make = _children => component((_: Hooks.empty) => listToElement([]));
  let createElement = (~key=?, ~children as _children, ()) =>
    element(~key?, make());
};

module EmptyComponentWithAlwaysEffect = {
  let component = component("Box");
  let make = (~onEffect, ~onEffectDispose, _children) => component((slots) => {
      let _slots: Hooks.empty = Hooks.effect(Always, () => {
        onEffect(); 
        Some(onEffectDispose);
      }, slots);
      listToElement([])
  });
  let createElement = (~key=?, ~children as _children, ~onEffect, ~onEffectDispose, ()) =>
    element(~key?, make(~onEffect, ~onEffectDispose, _children));
};

module EmptyComponentWithOnMountEffect = {
  let component = component("Box");
  let make = (~onEffect, ~onEffectDispose, _children) => component((slots) => {
      let _slots: Hooks.empty = Hooks.effect(OnMount, () => {
        onEffect(); 
        Some(onEffectDispose);
      }, slots);
      listToElement([])
  });
  let createElement = (~key=?, ~children as _children, ~onEffect, ~onEffectDispose, ()) =>
    element(~key?, make(~onEffect, ~onEffectDispose, _children));
};
