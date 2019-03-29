open Brisk; 

let component = nativeComponent("activityIndicator");

type attribute = [ Layout.style | Styles.viewStyle];
type style = list(attribute);

let activityIndicator = (~style: style=[], ~children as _: list(unit), ()) =>
  component(hooks =>
    (
      hooks,
      {
        make: () => {
          let view = BriskProgressIndicator.make();
          let layoutNode =
            Layout.Node.make(
              ~style,
              {view, isYAxisFlipped: false},
            );
          {view, layoutNode};
        },
        configureInstance: (~isFirstRender as _, {view} as node) => {
          style
          |> List.iter(attribute =>
               switch (attribute) {
               | #Styles.viewStyle as attr => Styles.setViewStyle(view, attr)
               | #Layout.style => ()
               }
             );
          node;
        },
        children: Brisk.empty,
      },
    )
  );
