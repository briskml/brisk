type t = CocoaClass.view;

[@noalloc] external make: unit => t = "ml_NSView_make";
[@noalloc] external addSubview: (t, t) => unit = "ml_NSView_addSubview";
[@noalloc] external removeSubview: t => unit = "ml_NSView_removeSubview";

[@noalloc]
external setFrame:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_NSView_setFrame_bc" "ml_NSView_setFrame";

[@noalloc]
external setBorderWidth: (CocoaClass.view, [@unboxed] float) => unit =
  "ml_NSView_setBorderWidth_bc" "ml_NSView_setBorderWidth";

[@noalloc]
external setBorderColor:
  (
    CocoaClass.view,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_NSView_setBorderColor_bc" "ml_NSView_setBorderColor";

[@noalloc]
external setBackgroundColor:
  (
    CocoaClass.view,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_NSView_setBackgroundColor_bc" "ml_NSView_setBackgroundColor";
