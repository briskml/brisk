open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let measure = (node, _, _, _, _) => {
  open Layout.FlexLayout.LayoutSupport.LayoutTypes;

  let {context: {view: img}}: node = node;

  let width = BriskImage.getImageWidth(img);
  let height = BriskImage.getImageHeight(img);

  {width, height};
};

let component = nativeComponent("image");

let component = (~style: style=[], ~source, ~children as _: list(unit), ()) =>
  component(hooks =>
    (
      hooks,
      {
        make: () => {
          let view = BriskImage.make(~source, ());
          let layoutNode =
            Layout.Node.make(
              ~measure,
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
