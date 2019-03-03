open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("clickable");
  (~style=[], ~onClick=() => (), ~children: list(Brisk.syntheticElement), ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskClickable.make(~onClick, ());
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: false});

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | #Styles.viewStyle => Styles.setViewStyle(view, attribute)
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
