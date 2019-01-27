open Brisk;
open Layout;

type attr = [ Layout.style | Styles.viewStyle];

type style = list(attr);

let component = nativeComponent("View");

let make = (~style: style=[], children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskView.make();
        {view, layoutNode: makeLayoutNode(~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attr =>
             switch (attr) {
             | #Styles.viewStyle => Styles.setViewStyle(view, attr)
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
