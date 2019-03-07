open Brisk;

type attribute = [ View.attribute];

type style = list(attribute);

let scrollableArea = {
  let component = nativeComponent("ScrollView");
  (
    ~onScroll,
    ~onReachedEnd, 
    ~style: style=[],
    ~contentStyle: style=[],
    ~children: list(Brisk.syntheticElement),
    (),
  ) =>
    component(hooks =>{ 

     let (lastScrollPosition, _, hooks) = Hooks.state(ref(0.), hooks);
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
           (switch (onReachedEnd, onScroll) {
            | (Some(onReachedEnd), Some(onScroll)) => Some((x, y, contentWidth, contentHeight, visibleWidth, visibleHeight) => {
             let maxY = y +. visibleHeight; 
             if (lastScrollPosition^ +. 100. < maxY && y +. 100. > maxY) {
              onReachedEnd();
             }
             onScroll(x, y, contentWidth, contentHeight, visibleWidth, visibleHeight); 
             lastScrollPosition := (y); 
            })
            | (Some(onReachedEnd), None) => Some((_, y, _, contentHeight, _, visibleHeight) => {
             let maxY = y +. visibleHeight; 
             if (lastScrollPosition^ +. 100. < contentHeight && maxY +. 100. > contentHeight) {
              onReachedEnd();
             }
           lastScrollPosition := maxY; 
            }) 
            | (None, Some(onScroll)) => Some(onScroll) 
            | (None, None) => None 
            })|> BriskScrollView.setOnScroll(view); 
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
    } 
    );
};

let component = (
    ~onScroll=?,
    ~onReachedEnd=?, 
    ~style: style=[], 
    ~children: list(Brisk.syntheticElement), 
    ()
  ) => {
  open Brisk.Layout;

  let view = View.component;

  let scrollStyle = [flex(1.)];
  let contentStyle = [position(~top=0., ~left=0., ~right=0., `Absolute)];

  <view style=scrollStyle>
    <scrollableArea onScroll onReachedEnd style=scrollStyle contentStyle>
      <view style> ...children </view>
    </scrollableArea>
  </view>;
};
