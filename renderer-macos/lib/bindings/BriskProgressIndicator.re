type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskProgressIndicator_make";

[@noalloc]
external fittingWidth: t => [@unboxed] float =
  "ml_BriskProgressIndicator_fittingWidth_bc"
  "ml_BriskProgressIndicator_fittingWidth";
[@noalloc]
external fittingHeight: t => [@unboxed] float =
  "ml_BriskProgressIndicator_fittingWidth_bc"
  "ml_BriskProgressIndicator_fittingWidth";
