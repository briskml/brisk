open Brisk;

type attribute = [ View.attribute];

type style = list(attribute);

let component = {
  let component = nativeComponent("ScrollView");
  (~style: style=[], ~children, ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            open Layout;
            open Layout.Node;
            let view = BriskScrollView.make();
            let container = Composite.makeFlexNode(~style, view);
            let content =
              Composite.makeFlexNode(
                ~style=[position(~top=0., ~left=0., ~right=0., `Absolute)],
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
          children: listToElement(children),
        },
      )
    );
};
