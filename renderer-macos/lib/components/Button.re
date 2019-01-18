open Brisk;
open Layout;

type attr = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attr);

let component = nativeComponent("Button");
let make =
    (~type_=?, ~bezel=?, ~title=?, ~style=[], ~callback=() => (), children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let btn =
          BriskButton.(
            make(~type_?, ~bezel?, ~title?, ~onClick=callback, ())
          );
        {view: btn, layoutNode: makeLayoutNode(~style, btn)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attr =>
             switch (attr) {
             | `Background(_) =>
               BriskButton.setIsBordered(view, false);
               Styles.setViewStyle(view, attr);
             | #Styles.textStyle => Styles.setTextStyle(view, attr)
             | #Styles.viewStyle => Styles.setViewStyle(view, attr)
             | #Layout.style => ()
             }
           );
        Styles.flushTextStyle(view);
        node;
      },
      children,
    }
  );

let createElement =
    (~type_=?, ~bezel=?, ~style=[], ~title=?, ~callback=?, ~children, ()) =>
  element(
    make(
      ~type_?,
      ~bezel?,
      ~style,
      ~title?,
      ~callback?,
      listToElement(children),
    ),
  );
