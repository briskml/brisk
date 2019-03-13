open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("button");
  (
    ~type_=?,
    ~bezel=?,
    ~title=?,
    ~style: style=[],
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
            Styles.beginTextStyleChanges(view);
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | `Background(_) as attr =>
                   BriskButton.setIsBordered(view, false);
                   Styles.setViewStyle(view, attr);
                 | #Styles.textStyle as attr =>
                   Styles.setTextStyle(view, attr)
                 | #Styles.viewStyle as attr =>
                   Styles.setViewStyle(view, attr)
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
