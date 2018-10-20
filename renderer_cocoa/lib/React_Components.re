open React;
open Cocoa;

type color = {
  red: float,
  green: float,
  blue: float,
  alpha: float,
};
type style = {
  backgroundColor: color,
  borderWidth: float,
  borderColor: color,
};

module View = {
  let component = statelessNativeComponent("NSView");
  let make = (~layout, ~style, children) => {
    ...component,
    render: _ => {
      make: () => {
        let view = NSView.make();
        {view, layoutNode: makeLayoutNode(~layout, view)};
      },
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
      updateInstance: (_self, {view}) => {
        let {red, green, blue, alpha} = style.backgroundColor;
        NSView.setBackgroundColor(view, red, green, blue, alpha);
        NSView.setBorderWidth(view, style.borderWidth);
        let {red, green, blue, alpha} = style.borderColor;
        NSView.setBorderColor(view, red, green, blue, alpha);
      },
      children,
    },
  };
  let createElement = (~layout, ~style, ~children, ()) =>
    element(make(~layout, ~style, listToElement(children)));
};

module Button = {
  let component = statelessNativeComponent("Button");
  let make = (~title=?, ~layout, ~style=?, ~callback=() => (), children) => {
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
        switch (style) {
        | Some(style) =>
          let {red, green, blue, alpha} = style.backgroundColor;

          NSView.setBackgroundColor(view, red, green, blue, alpha);
          NSView.setBorderWidth(view, style.borderWidth);
          let {red, green, blue, alpha} = style.borderColor;
          NSView.setBorderColor(view, red, green, blue, alpha);
        | None => ignore()
        },
      children,
    },
  };
  let createElement =
      (~layout, ~style=?, ~title=?, ~callback=?, ~children, ()) =>
    element(
      make(~layout, ~style?, ~title?, ~callback?, listToElement(children)),
    );
};
