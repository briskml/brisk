open Brisk;

type attribute = [ Layout.style | BriskSplitView.style];

type style = list(attribute);

let dividerStyle = dividerStyle => `DividerStyle(dividerStyle);
let vertical = vertical => `Vertical(vertical);

let component = nativeComponent("SplitView");

let make = (~style: style=[], children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskSplitView.make();
        {view, layoutNode: Layout.Node.make(~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attribute =>
             switch (attribute) {
             | #BriskSplitView.style =>
               BriskSplitView.setStyle(view, attribute)
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
