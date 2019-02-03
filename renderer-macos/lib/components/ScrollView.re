open Brisk;

type attribute = [ View.attribute];

type style = list(attribute);

let component = nativeComponent("ScrollView");

let make = (~style: style=[], children: Brisk.syntheticElement) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        open Layout.Node;
        let view = BriskScrollView.make();
        let container = Composite.makeFlexNode(~style, view);
        let content =
          Composite.makeFlexNode(
            ~style=
              [
                Layout.position(~top=0., ~left=0., ~right=0., `Absolute),
              ],
            BriskScrollView.documentView(view),
          );
        insertChild(container, content, 0);
        {view, layoutNode: Composite.make(~container, ~content)};
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
