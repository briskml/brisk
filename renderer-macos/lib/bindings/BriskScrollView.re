type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskScrollView_make";

[@noalloc] external documentView: t => t = "ml_BriskScrollView_documentView";

/**
  * setOnScroll [scrollView, (originX, origin.y, contentViewWidth, contentViewHeight)]
  *
  */
external setOnScroll: (t, option((float, float, float, float) => unit)) => unit = "ml_BriskScrollView_setOnScroll";
