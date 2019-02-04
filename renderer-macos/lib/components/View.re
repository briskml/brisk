open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = nativeComponent("View");

let make = (~style: style=[], children) =>
  component((_: Hooks.empty) =>
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
      children,
    }
  );

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, listToElement(children)));
