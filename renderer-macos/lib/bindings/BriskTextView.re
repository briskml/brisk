type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskTextView_make";

[@noalloc]
external getTextWidth: t => [@unboxed] float =
  "ml_BriskTextView_getTextWidth_bc" "ml_BriskTextView_getTextWidth";

[@noalloc]
external getTextHeight: t => [@unboxed] float =
  "ml_BriskTextView_getTextHeight_bc" "ml_BriskTextView_getTextHeight";

[@noalloc]
external setStringValue: (t, string) => unit =
  "ml_BriskTextView_setStringValue";

[@noalloc]
external setFont: (t, string, [@unboxed] float, [@unboxed] float) => unit =
  "ml_BriskTextView_setFont" "ml_BriskTextView_setFont";

[@noalloc]
external setKern: (t, [@unboxed] float) => unit =
  "ml_BriskTextView_setKern_bc" "ml_BriskTextView_setKern";

[@noalloc]
external setColor:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskTextView_setColor_bc" "ml_BriskTextView_setColor";

[@noalloc]
external setAlignment: (t, [@untagged] int) => unit =
  "ml_BriskTextView_setAlignment_bc" "ml_BriskTextView_setAlignment";

[@noalloc]
external setLineBreak: (t, [@untagged] int) => unit =
  "ml_BriskTextView_setLineBreakMode_bc" "ml_BriskTextView_setLineBreakMode";

[@noalloc]
external setLineSpacing: (t, [@unboxed] float) => unit =
  "ml_BriskTextView_setLineSpacing_bc" "ml_BriskTextView_setLineSpacing";

[@noalloc]
external setCornerRadius: (t, [@unboxed] float) => unit =
  "ml_BriskTextView_setCornerRadius_bc" "ml_BriskTextView_setCornerRadius";

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
  "ml_BriskTextView_setBackgroundColor_bc"
  "ml_BriskTextView_setBackgroundColor";

[@noalloc]
external setPadding:
  (
    t,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskTextView_setPadding_bc" "ml_BriskTextView_setPadding";

let make = str => {
  let txt = make();

  setStringValue(txt, str);
  txt;
};
