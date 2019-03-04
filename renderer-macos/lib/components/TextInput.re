open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

let component = nativeComponent("Text");

let measure = (node, _, _, _, _) => {
  open Layout.FlexLayout.LayoutSupport.LayoutTypes;

  let {context: input}: node = node;

  let width = BriskTextInput.getTextWidth(input) |> int_of_float;
  let height = BriskTextInput.getTextHeight(input) |> int_of_float;

  {width, height};
};

let make = (~style=[], ~value, ~placeholder, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskTextInput.make(value, placeholder);
        {view, layoutNode: Layout.Node.make(~measure, ~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        open Layout;
        style
        |> List.iter(attribute =>
             switch (attribute) {
             | `Padding({left, top, right, bottom}) =>
               BriskTextInput.setPadding(view, left, top, right, bottom)
             | `Background(({r, g, b, a}: Color.t)) =>
               BriskTextInput.setBackgroundColor(view, r, g, b, a)
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

let createElement = (~style=[], ~value, ~placeholder, ~children, ()) =>
  element(make(~style, ~value, ~placeholder, listToElement(children)));
