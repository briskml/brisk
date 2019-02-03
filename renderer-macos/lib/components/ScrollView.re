open Brisk;
open Layout;

type attr = [ View.attr];

type style = list(attr);

module ScrollableArea = {
  let component = nativeComponent("ScrollView");

  let make = (~style: style=[], children: Brisk.syntheticElement) =>
    component((_: Hooks.empty) =>
      {
        make: () => {
          let view = BriskScrollView.make();
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
};

let component = component("ScrollView");

let make = (~style: style=[], children: Brisk.syntheticElement) =>
  component((_: Hooks.empty) => {
    let scrollStyle = [flexGrow(1.), overflow(`Scroll)];
    let contentStyle = [position(~top=0., ~left=0., ~right=0., `Absolute)];

    <View style>
      <ScrollableArea style=scrollStyle>
        <View style=contentStyle> children </View>
      </ScrollableArea>
    </View>;
  });

let createElement = (~style=[], ~children, ()) =>
  element(make(~style, listToElement(children)));
