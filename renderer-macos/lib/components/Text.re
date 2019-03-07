open Brisk;

type attribute = [ Layout.style | Styles.textStyle | Styles.viewStyle];

type style = list(attribute);

open Layout.FlexLayout.LayoutSupport.LayoutTypes; 

let shouldMeasureForMode = 
fun 
| Exactly => false
| CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS => raise(Invalid_argument("not implemented"))
| Undefined => true
| AtMost => true;

let maxContainerSizeForMode = (mode, size) =>
  switch (mode) {
  | Exactly => float_of_int(size);
  | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS => raise(Invalid_argument("not implemented"));
  | Undefined => max_float;
  | AtMost => float_of_int(size);
};

let measure = (node, width, widthMode, height, heightMode) => {
  let shouldCalculateWidth = shouldMeasureForMode(widthMode);
  let shouldCalculateHeight = shouldMeasureForMode(heightMode);
  if (shouldCalculateWidth || shouldCalculateHeight) {
    let containerWidth = maxContainerSizeForMode(widthMode, width);
    let containerHeight = maxContainerSizeForMode(heightMode, height);
    BriskTextView.setTextContainerSize(node.context.view, containerWidth, containerHeight);
    {
      width: (shouldCalculateWidth ? 
    
    int_of_float(BriskTextView.getTextWidth(node.context.view)) : 
    width),
    height: (shouldCalculateHeight ? 
    (BriskTextView.getTextHeight(node.context.view) |> int_of_float) 
    : height)};
  } else {
    {width, height}
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
                 | `Padding({left, top, right, bottom}) =>
                   BriskTextView.setPadding(view, left, top, right, bottom)
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
