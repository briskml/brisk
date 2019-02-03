open Brisk;
open Layout;

type attribute = [ View.attribute];

type style = list(attribute);

let component = nativeComponent("ScrollView");

let make = (~style: style=[], children: Brisk.syntheticElement) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskScrollView.make();
        let container = LayoutNode.Composite.makeFlexNode(~style, view);
        let middle =
          LayoutNode.Composite.makeFlexNode(
            ~style,
            Layout.FlexLayout.LayoutSupport.theNullNode.context,
          );
        let content =
          LayoutNode.Composite.makeFlexNode(
            ~style=
              Layout.Attributes.[
                position(~top=0., ~left=0., ~right=0., `Absolute),
              ],
            BriskScrollView.documentView(view),
          );
        LayoutNode.insertChild(container, middle, 0);
        LayoutNode.insertChild(middle, content, 0);
        {view, layoutNode: LayoutNode.Composite.make(~container, ~content)};
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
