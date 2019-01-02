open React;
open Cocoa;
open Layout;

type attr = [
  | `width(float)
  | `height(float)
  | `color(Color.t)
  | `background(Color.t)
  | `border(Border.t)
  | `padding(float)
  | `margin(float)
];

type style = list(attr);

let component = statelessNativeComponent("Button");
let make =
    (~type_=?, ~bezel=?, ~title=?, ~style=[], ~callback=() => (), children) => {
  ...component,
  render: _ => {
    make: () => {
      let btn =
        NSButton.(make(~type_?, ~bezel?, ~title?, ~onClick=callback, ()));
      {view: btn, layoutNode: makeLayoutNode(~style, btn)};
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
