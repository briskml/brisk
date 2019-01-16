open Brisk;
open Layout;

type attr = [ Layout.style | `Background(Color.t)];

type style = list(attr);

let component = statelessNativeComponent("View");
let make = (~style: style=[], children) => {
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
           | `Background(({r, g, b, a}: Color.t)) =>
             NSView.setBackgroundColor(view, r, g, b, a)
           | `Border(({width, color}: Border.t)) =>
             if (!isUndefined(width)) {
               NSView.setBorderWidth(view, width);
             };
             let {r, g, b, a}: Color.t = color;
             NSView.setBorderColor(view, r, g, b, a);
           | #Layout.style => ()
           }
         );
      node;
    },
    children,
  },
};

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, listToElement(children)));
