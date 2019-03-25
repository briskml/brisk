type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskTextView_make";

[@noalloc]
external setTextContainerSize: (t, [@unboxed] float, [@unboxed] float) => unit =
  "ml_BriskTextView__setTextContainerSize_bc"
  "ml_BriskTextView_setTextContainerSize";

[@noalloc]
external getTextWidth: t => [@unboxed] float =
  "ml_BriskTextView_getTextWidth_bc" "ml_BriskTextView_getTextWidth";

[@noalloc]
external getTextHeight: t => [@unboxed] float =
  "ml_BriskTextView_getTextHeight_bc" "ml_BriskTextView_getTextHeight";

[@noalloc]
external setStringValue: (t, string) => unit =
  "ml_BriskTextView_setStringValue";

[@noalloc] external setHtml: (t, string) => unit = "ml_BriskTextView_setHtml";

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
external setSelectable: (t, [@untagged] int) => unit =
  "ml_BriskTextView_setSelectable_bc" "ml_BriskTextView_setSelectable";

let setSelectable = (txt, selectable) =>
  setSelectable(txt, selectable ? 1 : 0);

let make = (~html=false, str) => {
  let txt = make();

  if (html) {
    setHtml(txt, str);
  } else {
    setStringValue(txt, str);
  };

  txt;
};
