open React;
open Cocoa;
open Layout;

type attr = [
  Layout.style
  | `font(Font.t)
  | `background(Color.t)
  | `color(Color.t)
];

type style = list(attr);

let component = statelessNativeComponent("Text");

let make = (~style=[], ~value, children) => {
  ...component,
  render: _ => {
    make: () => {
      let view = NSTextView.make(value);
      {view, layoutNode: makeLayoutNode(~style, view)};
    },
    shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
    updateInstance: (_self, {view} as node) => {
      style
      |> List.iter(attr =>
           switch (attr) {
           | `font(_) => ()
           | `background(({r, g, b, a}: Color.t)) =>
             NSTextView.setBackgroundColor(view, r, g, b, a)
           | `color(_) => view |> ignore
           | #Layout.style => ()
           }
         );
      node;
    },
    children,
  },
};

let createElement = (~style=[], ~value, ~children, ()) =>
  element(make(~style, ~value, listToElement(children)));
