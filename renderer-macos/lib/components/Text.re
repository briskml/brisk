open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

open Layout.FlexLayout.LayoutSupport.LayoutTypes;

let shouldMeasureForMode =
  fun
  | Exactly => false
  | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS =>
    raise(Invalid_argument("not implemented"))
  | Undefined => true
  | AtMost => true;

let maxContainerSizeForMode = (mode, size) =>
  switch (mode) {
  | Exactly => size
  | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS =>
    raise(Invalid_argument("not implemented"))
  | Undefined => max_float
  | AtMost => size
  };

let measure = (node, width, widthMode, height, heightMode) => {
  let shouldCalculateWidth = shouldMeasureForMode(widthMode);
  let shouldCalculateHeight = shouldMeasureForMode(heightMode);
  if (shouldCalculateWidth || shouldCalculateHeight) {
    let containerWidth = maxContainerSizeForMode(widthMode, width);
    let containerHeight = maxContainerSizeForMode(heightMode, height);
    BriskTextView.setTextContainerSize(
      node.context.view,
      containerWidth,
      containerHeight,
    );
    let paddingHorizontal =
      Layout.get0IfUndefined(node.style.paddingLeft)
      +. Layout.get0IfUndefined(node.style.paddingRight);
    let paddingVertical =
      Layout.get0IfUndefined(node.style.paddingTop)
      +. Layout.get0IfUndefined(node.style.paddingBottom);
    {
      width:
        shouldCalculateWidth
          ? BriskTextView.getTextWidth(node.context.view)
            +. paddingHorizontal
          : width,
      height:
        shouldCalculateHeight
          ? BriskTextView.getTextHeight(node.context.view) +. paddingVertical
          : height,
    };
  } else {
    {width, height};
  };
};

let component = {
  let component = nativeComponent("text");
  (~style: style=[], ~value, ~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let view = BriskTextView.make(value);
            let layoutNode =
              Layout.Node.make(
                ~measure,
                ~style,
                {view, isYAxisFlipped: true},
              );

            {view, layoutNode};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            open Layout;
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | `Background(({r, g, b, a}: Color.t)) =>
                   BriskTextView.setBackgroundColor(view, r, g, b, a)
                 | #Styles.viewStyle as attr =>
                   Styles.setViewStyle(view, attr)
                 | #Styles.textStyle as attr =>
                   Styles.setTextStyle(view, attr)
                 | #Layout.style => ()
                 }
               );
            Styles.commitTextStyle(view);
            node;
          },
          children: Brisk.empty,
        },
      )
    );
};
