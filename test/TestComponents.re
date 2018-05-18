module Box = {
  let createElement = (~id, ~state="ImABox", ~children, ()) =>
    TestRenderer.{
      component: Component(Components.Box.component),
      id,
      state,
      subtree: children
    };
};

module BoxWithDynamicKeys = {
  let createElement = (~id, ~state="ImABox", ~children, ()) =>
    TestRenderer.{
      component: Component(Components.BoxWithDynamicKeys.component),
      id,
      state,
      subtree: children
    };
};

module BoxList = {
  let createElement = (~id, ~state="", ~children, ()) =>
    TestRenderer.{
      component: Component(Components.BoxList.component),
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

module Text = {
  open Components;
  let createElement = (~id, ~title, ~children: _, ()) =>
    TestRenderer.{
      id,
      component: Component(Text.component),
      state: title,
      subtree: []
    };
};

module ButtonWrapper = {
  open Components;
  let createElement = (~id, ~children: _, ()) =>
    TestRenderer.{
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
              subtree: []
            }
          ]
        }
      ]
    };
};

module ButtonWrapperWrapper = {
  let createElement = (~id, ~nestedText, ~children: _, ()) =>
    TestRenderer.{
      id,
      component: Component(Components.ButtonWrapperWrapper.component),
      state: "",
      subtree: [
        <Div id=(id + 1)>
          <Text id=(id + 2) title="buttonWrapperWrapperState" />
          <Text id=(id + 3) title=nestedText />
          <ButtonWrapper id=(id + 4) />
        </Div>
      ]
    };
};

module UpdateAlternateClicks = {
  let createElement = (~id=1, ~state, ~text, ~children: _, ()) =>
    TestRenderer.{
      id,
      component: Component(Components.UpdateAlternateClicks.component),
      state,
      subtree: [<Text id=(id + 1) title=text />]
    };
};
