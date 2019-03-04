type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskTextInput_make";

[@noalloc]
external getTextWidth: t => [@unboxed] float =
  "ml_BriskTextInput_getTextWidth_bc" "ml_BriskTextInput_getTextWidth";

[@noalloc]
external getTextHeight: t => [@unboxed] float =
  "ml_BriskTextInput_getTextHeight_bc" "ml_BriskTextInput_getTextHeight";

[@noalloc]
external setStringValue: (t, string) => unit =
  "ml_BriskTextInput_setStringValue";

[@noalloc]
external setPlaceholder: (t, string) => unit =
  "ml_BriskTextInput_setPlaceholder";

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
  "ml_BriskTextInput_setBackgroundColor_bc"
  "ml_BriskTextInput_setBackgroundColor";

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
  "ml_BriskTextInput_setPadding_bc" "ml_BriskTextInput_setPadding";

let make = (value, placeholder) => {
  let input = make();

  setStringValue(input, value);
  setPlaceholder(input, placeholder);
  input;
};
