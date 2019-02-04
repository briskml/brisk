type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskScrollView_make";

[@noalloc] external documentView: t => t = "ml_BriskScrollView_documentView";
