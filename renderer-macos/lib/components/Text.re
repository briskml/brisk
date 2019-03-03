open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

let measure = (node, _, _, _, _) => {
  open Layout.FlexLayout.LayoutSupport.LayoutTypes;

  let {context: {view: txt}}: node = node;

  let width = BriskTextView.getTextWidth(txt) |> int_of_float;
  let height = BriskTextView.getTextHeight(txt) |> int_of_float;

  {width, height};
};

let component = {
  let component = nativeComponent("text");
  (~style=[], ~value, ~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskTextView.make(value);
            let layoutNode =
              Layout.Node.make(
                ~measure,
                ~style,
                {view, isYAxisFlipped: true},
              );

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            open Layout;
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
          children: Brisk.empty,
        },
      )
    );
};
