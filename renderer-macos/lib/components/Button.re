open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("button");
  (
    ~type_=?,
    ~bezel=?,
    ~title=?,
    ~style=[],
    ~onClick=() => (),
    ~children as _: list(unit),
    (),
  ) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view =
              BriskButton.make(~type_?, ~bezel?, ~title?, ~onClick, ());
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: true});

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | `Background(_) =>
                   BriskButton.setIsBordered(view, false);
                   Styles.setViewStyle(view, attribute);
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
