open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("view");
  (~style: style=[], ~children, ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskView.make();
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: false});
            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
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
          children: listToElement(children),
        },
      )
    );
};
