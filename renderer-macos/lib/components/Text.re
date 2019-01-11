open Brisk;
open Layout;

type attr = [
  Layout.style
  | `Font(Font.t)
  | `Kern(float)
  | `Align(Alignment.t)
  | `LineBreak(LineBreak.t)
  | `LineSpacing(float)
  | `CornerRadius(float)
  | `Background(Color.t)
  | `Color(Color.t)
];

type style = list(attr);

let component = nativeComponent("Text");

let measure = (node, _, _, _, _) => {
  open LayoutSupport.LayoutTypes;

  let {context: txt}: node = node;

  let width = BriskTextView.getTextWidth(txt) |> int_of_float;
  let height = BriskTextView.getTextHeight(txt) |> int_of_float;

  {width, height};
};

let make = (~style=[], ~value, children) =>
  component((_: Slots.empty) =>
    {
      make: () => {
        let view = BriskTextView.make(value);
        {view, layoutNode: makeLayoutNode(~measure, ~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
      style
      |> List.iter(attr =>
           switch (attr) {
           | `Font(({family, size, weight}: Font.t)) =>
             let weight =
               switch (weight) {
               | `UltraLight => (-0.8)
               | `Thin => (-0.6)
               | `Light => (-0.4)
               | `Regular => 0.
               | `Medium => 0.23
               | `Semibold => 0.3
               | `Bold => 0.4
               | `Heavy => 0.56
               | `Black => 0.62
               };
             BriskTextView.setFont(view, family, size, weight);
           | `Kern(kern) => BriskTextView.setKern(view, kern)
           | `Align(align) =>
             BriskTextView.setAlignment(
               view,
               switch (align) {
               | `Left => 0
               | `Right => 1
               | `Center => 2
               | `Justified => 3
               | `Natural => 4
               },
             )
           | `LineBreak(mode) => BriskTextView.setLineBreak(view, mode)
           | `LineSpacing(spacing) =>
             BriskTextView.setLineSpacing(view, spacing)
           | `CornerRadius(r) => BriskTextView.setCornerRadius(view, r)
           | `Color(({r, g, b, a}: Color.t)) =>
             BriskTextView.setColor(view, r, g, b, a)
           | `Background(({r, g, b, a}: Color.t)) =>
             BriskTextView.setBackgroundColor(view, r, g, b, a)
           | `Padding({left, top, right, bottom}) =>
             BriskTextView.setPadding(view, left, top, right, bottom)
           | #Layout.style => ()
           }
         );
      node;
    },
      children,
    }
  );

let createElement = (~style=[], ~value, ~children, ()) =>
  element(make(~style, ~value, listToElement(children)));
