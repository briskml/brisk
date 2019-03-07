open Brisk;
open Brisk.Layout;

type effectViewStyle = [
  | `Border(Border.t)
  | `Overflow(FlexLayout.LayoutSupport.LayoutTypes.overflow)
  | `Shadow(Shadow.t)
];

type attribute = [ Layout.style | effectViewStyle | BriskEffectView.style];

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
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: false});

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | #effectViewStyle as attr => Styles.setViewStyle(view, attr)
                 | #BriskEffectView.style as attr =>
                   BriskEffectView.setStyle(view, attr)
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
