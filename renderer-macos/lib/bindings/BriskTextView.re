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
