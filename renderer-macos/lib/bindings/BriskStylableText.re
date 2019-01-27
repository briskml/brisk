type text = CocoaTypes.view;

[@noalloc]
external applyChanges: text => unit =
  "ml_BriskStylableText_applyChanges" "ml_BriskStylableText_applyChanges";

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
external setLineBreak: (text, [@untagged] int) => unit =
  "ml_BriskStylableText_setLineBreakMode_bc"
  "ml_BriskStylableText_setLineBreakMode";

[@noalloc]
external setLineSpacing: (text, [@unboxed] float) => unit =
  "ml_BriskStylableText_setLineSpacing_b\n\n\n\n\n\n\nc"
  "ml_BriskStylableText_setLineSpacing";
