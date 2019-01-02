open React;
open Cocoa;
open Layout;

type attr = [
  | `width(float)
  | `height(float)
  | `font(Font.t)
  | `color(Color.t)
  | `padding(float)
  | `margin(float)
];

type style = list(attr);

let component = statelessNativeComponent("TextView");

let make = (~style=[], _children: string) => {
  ...component,
  render: _ => {
    make: () => {
      let view = NSView.make();
      {view, layoutNode: makeLayoutNode(~style, view)};
    },
    shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
    updateInstance: (_self, {view} as node) => {
      style
      |> List.iter(attr =>
           switch (attr) {
           | `font(_) => ()
           | `color(_) => view |> ignore
           | _ => ()
           }
         );
      node;
    },
    children: listToElement([]),
  },
};

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, children));
