open Brisk;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = {
  let component = nativeComponent("view");
  (~style: style=[], ~children, ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskView.make();
            let layoutNode =
              Layout.Node.make(~style, {view, isYAxisFlipped: false});
            {view, layoutNode};
          },
          configureInstance:
            (~isFirstRender as _, {view, layoutNode: {container}} as node) => {
            let style =
              List.fold_left(
                (acc, attribute) =>
                  switch (attribute) {
                  | #Styles.viewStyle as attr =>
                    ignore(Styles.setViewStyle(view, attr));
                    acc;
                  | #Layout.style as attr =>
                    Layout.applyCommonStyle(acc, attr)
                  },
                Layout.FlexLayout.LayoutSupport.defaultStyle,
                style,
              );
            container.style = style;
            node;
          },
          children: listToElement(children),
        },
      )
    );
};
