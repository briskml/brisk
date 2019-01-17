open Brisk;
open Layout;

type attr = [ Layout.style | Styles.textStyle | `Background(Color.t)];

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
             | `Background(({r, g, b, a}: Color.t)) =>
               BriskButton.setIsBordered(view, false);
               BriskView.setBackgroundColor(view, r, g, b, a);
             | `Border(({width, color}: Border.t)) =>
               if (!isUndefined(width)) {
                 BriskView.setBorderWidth(view, width);
               };
               let {r, g, b, a}: Color.t = color;
               BriskView.setBorderColor(view, r, g, b, a);
             | #Styles.textStyle => Styles.setTextStyle(view, attr)
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
