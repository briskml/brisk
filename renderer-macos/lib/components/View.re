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
            {view, layoutNode: Layout.Node.make(~style, view)};
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
          children: listToElement(children),
        },
      )
    );
};
