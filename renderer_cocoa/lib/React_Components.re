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
      shouldReconfigureInstance: (~oldState: _, ~newState: _) => false,
      updateInstance: (self, {view}) => {
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