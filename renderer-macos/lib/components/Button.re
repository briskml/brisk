open Brisk;
open Layout;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

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
        {view: btn, layoutNode: LayoutNode.make(~style, btn)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attribute =>
             switch (attribute) {
             | `Background(_) =>
               BriskButton.setIsBordered(view, false);
               Styles.setViewStyle(view, attribute);
             | #Styles.textStyle => Styles.setTextStyle(view, attribute)
             | #Styles.viewStyle => Styles.setViewStyle(view, attribute)
             | #Layout.style => ()
             }
           );
        Styles.commitTextStyle(view);
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
