open React;
open Cocoa;
open Layout;

type attr = [ Layout.style | `color(Color.t) | `background(Color.t)];

type style = list(attr);

let component = nativeComponent("Button");
let make =
    (~type_=?, ~bezel=?, ~title=?, ~style=[], ~callback=() => (), children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let btn =
          BriskButton.(
            make(~type_?, ~bezel?, ~title?, ~onClick=callback, ())
          );
        {view: btn, layoutNode: makeLayoutNode(~style, btn)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attr =>
             switch (attr) {
             | `color(_) => ()
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

let createElement =
    (~type_=?, ~bezel=?, ~style=[], ~title=?, ~callback=?, ~children, ()) =>
  element(
    make(
      ~type_?,
      ~bezel?,
      ~style,
      ~title?,
      ~callback?,
      listToElement(children),
    ),
  );
