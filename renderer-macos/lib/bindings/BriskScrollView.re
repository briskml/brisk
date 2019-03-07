type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskScrollView_make";

[@noalloc] external documentView: t => t = "ml_BriskScrollView_documentView";

/**
  * setOnScroll [scrollView, (originX, origin.y, contentViewWidth, contentViewHeight, visibleWidth, visibleHeight)]
  *
  */
external setOnScroll: (t, option((float, float, float, float, float, float) => unit)) => unit = "ml_BriskScrollView_setOnScroll";

let setOnScroll = (view, callback) => {
  switch callback {
  | Some(f) => setOnScroll(view, Some((x, y, contentWidth, contentHeight, visibleWidth, visibleHeight) => UIEventCallback.make(() => f(x, y, contentWidth, contentHeight, visibleWidth, visibleHeight), ())))
  | None => setOnScroll(view, None)
  };
}; 
