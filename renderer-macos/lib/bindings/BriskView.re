type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskView_make";
[@noalloc]
external insertSubview: (t, t, [@untagged] int) => unit =
  "ml_BriskView_insertSubview_bc" "ml_BriskView_insertSubview";

[@noalloc] external removeSubview: t => unit = "ml_BriskView_removeSubview";

[@noalloc]
external setFrame:
  (
    t,
    /* x */
    [@unboxed] float,
    /* y */
    [@unboxed] float,
    /* width */
    [@unboxed] float,
    /* height */
    [@unboxed] float,
    /* paddingLeft */
    [@unboxed] float,
    /* paddingRight */
    [@unboxed] float,
    /* paddingBottom */
    [@unboxed] float,
    /* paddingTop */
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

[@noalloc]
external setShadowOffset: (t, [@unboxed] float, [@unboxed] float) => unit =
  "ml_BriskView_setShadowOffset_bc" "ml_BriskView_setShadowOffset";

[@noalloc]
external setShadowRadius: (t, [@unboxed] float) => unit =
  "ml_BriskView_setShadowRadius_bc" "ml_BriskView_setShadowRadius";

[@noalloc]
external setShadowOpacity: (t, [@unboxed] float) => unit =
  "ml_BriskView_setShadowOpacity_bc" "ml_BriskView_setShadowOpacity";

[@noalloc]
external setShadowColor:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskView_setShadowColor_bc" "ml_BriskView_setShadowColor";

[@noalloc]
external setMasksToBounds: (t, [@untagged] int) => unit =
  "ml_BriskView_setMasksToBounds_bc" "ml_BriskView_setMasksToBounds";

let setMasksToBounds = (view, masks) =>
  setMasksToBounds(view, masks ? 1 : 0);
