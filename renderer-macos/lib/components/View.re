open Brisk;
open Layout;

type attr = [ Layout.style | `Background(Color.t)];

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
             | `Background(({r, g, b, a}: Color.t)) =>
               BriskView.setBackgroundColor(view, r, g, b, a)
             | `Border(({width, color}: Border.t)) =>
               if (!isUndefined(width)) {
                 BriskView.setBorderWidth(view, width);
               };
               let {r, g, b, a}: Color.t = color;
               BriskView.setBorderColor(view, r, g, b, a);
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
