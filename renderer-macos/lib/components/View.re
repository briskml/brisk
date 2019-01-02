open React;
open Cocoa;
open Layout;

type attr = [
  | `width(float)
  | `height(float)
  | `background(Color.t)
  | `border(Border.t)
  | `padding(float)
  | `margin(float)
];

type style = list(attr);

let component = statelessNativeComponent("View");
let make = (~style=[], children) => {
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
           | `background(({r, g, b, a}: Color.t)) =>
             NSView.setBackgroundColor(view, r, g, b, a)
           | `border(({width, color}: Border.t)) =>
             if (!isUndefined(width)) {
               NSView.setBorderWidth(view, width);
             };
             let {r, g, b, a}: Color.t = color;
             NSView.setBorderColor(view, r, g, b, a);
           | _ => ()
           }
         );
      node;
    },
    children,
  },
};

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, listToElement(children)));
