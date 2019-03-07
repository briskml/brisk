open Brisk;

type attribute = [ View.attribute];

type style = list(attribute);

let scrollableArea = {
  let component = nativeComponent("ScrollView");
  (
    ~style: style=[],
    ~contentStyle: style=[],
    ~children: list(Brisk.syntheticElement),
    (),
  ) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            open Layout.Node;

            /* Native NSScrollView element */
            let view = BriskScrollView.make();
            let container =
              Composite.makeFlexNode(~style, {view, isYAxisFlipped: false});

            /* Native NSScrollView.documentView element */
            let content =
              Composite.makeFlexNode(
                ~style=contentStyle,
                {
                  view: BriskScrollView.documentView(view),
                  isYAxisFlipped: true,
                },
              );

            insertChild(container, content, 0);

            {view, layoutNode: Composite.make(~container, ~content)};
          },
          configureInstance: (~isFirstRender as _, {view} as node) => {
            style
            |> List.iter(attribute =>
                 switch (attribute) {
                 | #Styles.viewStyle as attr =>
                   Styles.setViewStyle(view, attr)
                 | #Layout.style => ()
                 }
               );
            node;
          },
          children: Brisk.listToElement(children),
        },
      )
    );
};

let component = (~style=[], ~children: list(Brisk.syntheticElement), ()) => {
  open Brisk.Layout;

  let view = View.component;

  let scrollStyle = [flex(1.)];
  let contentStyle = [position(~top=0., ~left=0., ~right=0., `Absolute)];

  <view style=scrollStyle>
    <scrollableArea style=scrollStyle contentStyle>
      <view style> ...children </view>
    </scrollableArea>
  </view>;
};
