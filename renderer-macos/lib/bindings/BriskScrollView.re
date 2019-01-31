type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskScrollView_make";

[@noalloc]
external setContentFrame:
  (
    t,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskScrollView_setContentFrame_bc" "ml_BriskScrollView_setContentFrame";
