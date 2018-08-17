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
      make: () => {view: NSView.make(), layoutNode: makeLayoutNode(~layout)},
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => false,
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
};

module Button = {
  let component = statelessNativeComponent("Button");
  let make = (~title=?, ~layout, ~style=?, ~callback as _=?, children) => {
    ...component,
    render: _ => {
      make: () => {
        let btn = NSButton.make();
        let btn =
          switch (title) {
          | Some(title) => NSButton.setTitle(btn, title)
          | None => btn
          };
        /* let btn =
           switch (callback) {
           | Some(callback) => NSButton.setCallback(btn, callback)
           | None => btn
           }; */

        {view: btn, layoutNode: makeLayoutNode(~layout)};
      },
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => false,
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
};
