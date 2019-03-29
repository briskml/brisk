type t = CocoaTypes.view;


[@noalloc] external make: unit => t = "ml_BriskImage_make";

[@noalloc]
external getImageWidth: t => [@unboxed] float =
  "ml_BriskImage_getImageWidth_bc" "ml_BriskImage_getImageWidth";

[@noalloc]
external getImageHeight: t => [@unboxed] float =
  "ml_BriskImage_getImageHeight_bc" "ml_BriskImage_getImageHeight";

let make = (~source=?, ()) => {
  let img = make();
  BriskImageHostingView.setSource(img, source);
  img;
};
