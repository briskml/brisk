open Brisk;

type attribute = [ Layout.style | Styles.viewStyle | BriskEffectView.style];

type style = list(attribute);

let material = material => `Material(material);
let blendingMode = blendingMode => `BlendingMode(blendingMode);
let emphasized = emphasized => `Emphasized(emphasized);
let effectState = effectState => `EffectState(effectState);

let component = {
  let component = nativeComponent("effectView");
  (~style: style=[], ~children, ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskEffectView.make();
            {view, layoutNode: Layout.Node.make(~style, view)};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | #Styles.viewStyle => Styles.setViewStyle(view, attribute)
                 | #BriskEffectView.style =>
                   BriskEffectView.setStyle(view, attribute)
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
