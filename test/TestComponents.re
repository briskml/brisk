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
