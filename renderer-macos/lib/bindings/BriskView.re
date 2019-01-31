type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskView_make";
[@noalloc] external addSubview: (t, t) => unit = "ml_BriskView_addSubview";
[@noalloc] external removeSubview: t => unit = "ml_BriskView_removeSubview";

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
  "ml_BriskView_setFrame_bc" "ml_BriskView_setFrame";

[@noalloc]
external setBorderWidth: (t, [@unboxed] float) => unit =
  "ml_BriskView_setBorderWidth_bc" "ml_BriskView_setBorderWidth";

[@noalloc]
external setBorderRadius: (t, [@unboxed] float) => unit =
  "ml_BriskView_setBorderRadius_bc" "ml_BriskView_setBorderRadius";

[@noalloc]
external setBorderColor:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskView_setBorderColor_bc" "ml_BriskView_setBorderColor";

[@noalloc]
external setBackgroundColor:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskView_setBackgroundColor_bc" "ml_BriskView_setBackgroundColor";
