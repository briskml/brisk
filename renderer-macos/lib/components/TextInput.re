open Brisk;

type attribute = [ Layout.style ];

type style = list(attribute);

let component = nativeComponent("TextInput");
let make = (~style= [], ~callback=() => (), children) =>
  component((_: Hooks.empty) =>
    {
     make: () => {
       let input =
         BriskTextInput.(
           make(~onChange=callback, ())
         );
     },
     configureInstance: (~isFirstRender as _, {view} as node) => {
       style
       |> List.iter(attribute =>
             switch (attribute) {
             | #Layout.style => ()
             }
           );
        node;
     },
     children,
    }
  );

let createElement = (~style=[], ~callback=?, ~children, ()) =>
  element(
    make(
     ~style,
     ~callback?,
     listToElement(children)
    )
  );
