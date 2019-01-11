open React;
open Cocoa;
open Layout;

type attr = [ Layout.style | `background(Color.t)];

type style = list(attr);

let component = nativeComponent("View");
let make = (~style: style=[], children) =>
  component((_: Slots.empty) =>
    {
      make: () => {
        let view = NSView.make();
        {view, layoutNode: makeLayoutNode(~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
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
             | #Layout.style => ()
             }
           );
        node;
      },
      children,
    }
  );

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, listToElement(children)));
