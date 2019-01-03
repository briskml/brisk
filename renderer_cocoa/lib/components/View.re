open React;
open Cocoa;
open Layout;
open Style;

type attr = [
  | `background(color)
  | `borderColor(color)
  | `borderWidth(width)
];

type style = list(attr);

let component = statelessNativeComponent("NSView");
let make = (~layout, ~style=[], children) => {
  ...component,
  render: _ => {
    make: () => {
      let view = NSView.make();
      {view, layoutNode: makeLayoutNode(~layout, view)};
    },
    shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
    updateInstance: (_self, {view} as node) => {
      style
      |> List.iter(attr =>
           switch (attr) {
           | `background(color) =>
             let (r, g, b, a) = convertColor(color);
             NSView.setBackgroundColor(view, r, g, b, a);
           | `borderColor(color) =>
             let (r, g, b, a) = convertColor(color);
             NSView.setBorderColor(view, r, g, b, a);
           | `borderWidth(width) =>
             let w = convertWidth(width);
             NSView.setBorderWidth(view, w);
           }
         );
      node;
    },
    children,
  },
};

let createElement = (~layout, ~style, ~children, ()) =>
  element(make(~layout, ~style, listToElement(children)));
