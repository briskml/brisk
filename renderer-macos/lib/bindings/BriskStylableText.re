type text = CocoaTypes.view;

module LineBreakMode = {
  type t = int;
  /* NSLineBreakByWordWrapping = 0 */
  let wordWrapping: t = 0;
  /* NSLineBreakByCharWrapping = 1 */
  let charWrapping: t = 1;
  /* NSLineBreakByClipping = 2 */
  let clipping: t = 2;
  /* NSLineBreakByTruncatingHead = 3 */
  let truncaingHead: t = 3;
  /* NSLineBreakByTruncatingTail = 4 */
  let truncatingTail: t = 4;
  /* NSLineBreakByTruncatingMiddle = 5 */
  let truncatingMiddle: t = 5;
};

[@noalloc]
external applyChanges: text => unit =
  "ml_BriskStylableText_applyChanges" "ml_BriskStylableText_applyChanges";

external beginChanges: text => unit =
  "ml_BriskStylableText_beginTextStyleChanges_bc" "ml_BriskStylableText_beginTextStyleChanges";

[@noalloc]
external setFont: (text, string, [@unboxed] float, [@unboxed] float) => unit =
  "ml_BriskStylableText_setFont" "ml_BriskStylableText_setFont";

[@noalloc]
external setKern: (text, [@unboxed] float) => unit =
  "ml_BriskStylableText_setKern_bc" "ml_BriskStylableText_setKern";

[@noalloc]
external setColor:
  (
    text,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float,
    [@unboxed] float
  ) =>
  unit =
  "ml_BriskStylableText_setColor_bc" "ml_BriskStylableText_setColor";

[@noalloc]
external setAlignment: (text, [@untagged] int) => unit =
  "ml_BriskStylableText_setAlignment_bc" "ml_BriskStylableText_setAlignment";

[@noalloc]
external setLineBreak: (text, [@untagged] LineBreakMode.t) => unit =
  "ml_BriskStylableText_setLineBreakMode_bc"
  "ml_BriskStylableText_setLineBreakMode";

[@noalloc]
external setLineSpacing: (text, [@unboxed] float) => unit =
  "ml_BriskStylableText_setLineSpacing_b\n\n\n\n\n\n\nc"
  "ml_BriskStylableText_setLineSpacing";
