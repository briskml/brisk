open React;
open Cocoa;
open Layout;
open Style;

type attr = [
  | `color(color)
  | `background(color)
  | `borderColor(color)
  | `borderWidth(width)
];

type style = list(attr);

let component = statelessNativeComponent("Button");
let make = (~title=?, ~layout, ~style=[], ~callback=() => (), children) => {
  ...component,
  render: _ => {
    make: () => {
      let btn = NSButton.make();
      switch (title) {
      | Some(title) => NSButton.setTitle(btn, title)
      | None => ()
      };
      NSButton.setCallback(btn, () => callback());

      {view: btn, layoutNode: makeLayoutNode(~layout, btn)};
    },
    shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
    updateInstance: (_self, {view}) =>
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
         ),
    children,
  },
};

let createElement = (~layout, ~style=?, ~title=?, ~callback=?, ~children, ()) =>
  element(
    make(~layout, ~style?, ~title?, ~callback?, listToElement(children)),
  );
