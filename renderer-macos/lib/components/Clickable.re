open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("clickable");
  (
    ~style: style=[],
    ~onClick=() => (),
    ~children: list(Brisk.syntheticElement),
    (),
  ) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskClickable.make();
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: false});

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            BriskClickable.setOnClick(view, onClick);

            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | #Styles.viewStyle as attr =>
                   Styles.setViewStyle(view, attr)
                 | #Layout.style => ()
                 }
               );
            node;
          },
          children: Brisk.listToElement(children),
        },
      )
    );
};
