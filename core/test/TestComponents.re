open TestRenderer;

module Box = {
  let createElement = (~id, ~state="ImABox", ~children, ()) => {
    component: Component(Components.Box.component),
    id,
    state,
    subtree: children,
  };
};

module BoxItemDynamic = {
  let createElement = (~id, ~state="ImABox", ~children, ()) => {
    component: Component(Components.BoxItemDynamic.component),
    id,
    state,
    subtree: children,
  };
};

module BoxList = {
  let createElement = (~id, ~state="", ~children, ()) => {
    component: Component(Components.BoxList.component),
    id,
    state,
    subtree: children,
  };
};

module Div = {
  let createElement = (~id, ~children, ()) => {
    component: Component(Components.Div.component),
    id,
    state: "",
    subtree: children,
  };
};

module BoxWrapper = {
  let createElement = (~id, ~children, ()) => {
    component: Component(Components.BoxWrapper.component),
    id,
    state: "",
    subtree: children,
  };
};

module ChangeCounter = {
  let createElement = (~id, ~label, ~counter, ~children as _, ()) => {
    component: Component(Components.ChangeCounter.component),
    id,
    state: Printf.sprintf("[%i, %s]", counter, label),
    subtree: [],
  };
};

module Text = {
  let createElement = (~id, ~title, ~children as _, ()) => {
    id,
    component: Component(Components.Text.component),
    state: title,
    subtree: [],
  };
};

module ButtonWrapper = {
  open Components;
  let createElement = (~id, ~children as _, ()) => {
    id,
    component: Component(ButtonWrapper.component),
    state: "",
    subtree: [
      {
        id: id + 1,
        component: Component(StatelessButton.component),
        state: "",
        subtree: [
          {
            component: Component(Div.component),
            state: "",
            id: id + 2,
            subtree: [],
          },
        ],
      },
    ],
  };
};

module ButtonWrapperWrapper = {
  let createElement = (~id, ~nestedText, ~children as _, ()) => {
    id,
    component: Component(Components.ButtonWrapperWrapper.component),
    state: "",
    subtree: [
      <Div id=(id + 1)>
        <Text id=(id + 2) title=nestedText />
        <ButtonWrapper id=(id + 3) />
      </Div>,
    ],
  };
};

module UpdateAlternateClicks = {
  let createElement = (~id=1, ~state, ~text, ~children as _, ()) => {
    id,
    component: Component(Components.UpdateAlternateClicks.component),
    state,
    subtree: [<Text id=(id + 1) title=text />],
  };
};

module ToggleClicks = {
  let createElement = (~id=1, ~state, ~children, ()) => {
    id,
    component: Component(Components.ToggleClicks.component),
    state,
    subtree: children,
  };
};
