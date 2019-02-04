open Brisk;

module Material = BriskEffectView.Material;
module BlendingMode = BriskEffectView.BlendingMode;

type attr = [ Layout.style | Styles.viewStyle];

type style = list(attr);

let component = nativeComponent("View");

let make = (~style: style=[], ~blendingMode, ~material, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskEffectView.make();
        {view, layoutNode: Layout.Node.make(~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        switch (blendingMode) {
        | Some(blendingMode) =>
          BriskEffectView.setBlendingMode(view, blendingMode)
        | _ => ()
        };
        switch (material) {
        | Some(material) => BriskEffectView.setMaterial(view, material)
        | _ => ()
        };
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

let createElement = (~style=[], ~children, ~blendingMode=?, ~material=?, ()) =>
  element(make(~style, ~blendingMode, ~material, listToElement(children)));
