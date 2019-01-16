open React;
open Cocoa;
open Layout;

type attr = [ Layout.style];

type style = list(attr);

let component = nativeComponent("Image");

let measure = (node, _, _, _, _) => {
  open LayoutSupport.LayoutTypes;

  let {context: img}: node = node;

  let width = BriskImage.getImageWidth(img) |> int_of_float;
  let height = BriskImage.getImageHeight(img) |> int_of_float;

  {width, height};
};

let make = (~style=[], ~source, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskImage.make(~source, ());
        {view, layoutNode: makeLayoutNode(~measure, ~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view: _} as node) => {
        style
        |> List.iter(attr =>
             switch (attr) {
             | #Layout.style => ()
             }
           );
        node;
      },
      children,
    }
  );

let createElement = (~style=[], ~source, ~children, ()) =>
  element(make(~style, ~source, listToElement(children)));
