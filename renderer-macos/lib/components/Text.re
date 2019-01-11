open React;
open Cocoa;
open Layout;

type attr = [
  Layout.style
  | `font(Font.t)
  | `kern(float)
  | `align(Alignment.t)
  | `lineBreak(LineBreak.t)
  | `lineSpacing(float)
  | `background(Color.t)
  | `color(Color.t)
];

type style = list(attr);

let component = statelessNativeComponent("Text");

let measure = (node, _, _, _, _) => {
  open LayoutSupport.LayoutTypes;

  let {context: txt}: node = node;

  let width = NSTextView.getTextWidth(txt) |> int_of_float;
  let height = NSTextView.getTextHeight(txt) |> int_of_float;

  {width, height};
};

let make = (~style=[], ~value, children) => {
  ...component,
  render: _ => {
    make: () => {
      let view = NSTextView.make(value);
      {view, layoutNode: makeLayoutNode(~measure, ~style, view)};
    },
    shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
    updateInstance: (_self, {view} as node) => {
      style
      |> List.iter(attr =>
           switch (attr) {
           | `font(({family, size, weight}: Font.t)) =>
             let weight =
               switch (weight) {
               | `ultraLight => (-0.8)
               | `thin => (-0.6)
               | `light => (-0.4)
               | `regular => 0.
               | `medium => 0.23
               | `semibold => 0.3
               | `bold => 0.4
               | `heavy => 0.56
               | `black => 0.62
               };
             NSTextView.setFont(view, family, size, weight);
           | `kern(kern) => NSTextView.setKern(view, kern)
           | `align(align) =>
             NSTextView.setAlignment(
               view,
               switch (align) {
               | `left => 0
               | `right => 1
               | `center => 2
               | `justified => 3
               | `natural => 4
               },
             )
           | `lineBreak(mode) => NSTextView.setLineBreak(view, mode)
           | `lineSpacing(spacing) =>
             NSTextView.setLineSpacing(view, spacing)
           | `color(({r, g, b, a}: Color.t)) =>
             NSTextView.setColor(view, r, g, b, a)
           | `background(({r, g, b, a}: Color.t)) =>
             NSTextView.setBackgroundColor(view, r, g, b, a)
           | #Layout.style => ()
           }
         );
      node;
    },
    children,
  },
};

let createElement = (~style=[], ~value, ~children, ()) =>
  element(make(~style, ~value, listToElement(children)));
