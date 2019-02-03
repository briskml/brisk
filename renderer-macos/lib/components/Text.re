open Brisk;
open Layout;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

let component = nativeComponent("Text");

let measure = (node, _, _, _, _) => {
  open LayoutSupport.LayoutTypes;

  let {context: txt}: node = node;

  let width = BriskTextView.getTextWidth(txt) |> int_of_float;
  let height = BriskTextView.getTextHeight(txt) |> int_of_float;

  {width, height};
};

let make = (~style=[], ~value, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskTextView.make(value);
        {view, layoutNode: makeLayoutNode(~measure, ~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attribute =>
             switch (attribute) {
             | `Padding({left, top, right, bottom}) =>
               BriskTextView.setPadding(view, left, top, right, bottom)
             | `Background(({r, g, b, a}: Color.t)) =>
               BriskTextView.setBackgroundColor(view, r, g, b, a)
             | #Styles.textStyle => Styles.setTextStyle(view, attribute)
             | #Styles.viewStyle => Styles.setViewStyle(view, attribute)
             | #Layout.style => ()
             }
           );
        Styles.commitTextStyle(view);
        node;
      },
      children,
    }
  );

let createElement = (~style=[], ~value, ~children, ()) =>
  element(make(~style, ~value, listToElement(children)));
