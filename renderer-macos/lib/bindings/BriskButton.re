type t = CocoaTypes.view;

type buttonType =
  | MomentaryLight
  | PushOnPushOff
  | Toggle
  | Switch
  | Radio
  | MomentaryChange
  | OnOff
  | MomentaryPushIn;

type bezelStyle =
  | Rounded
  | RegularSquare
  | ThickSquare
  | ThickerSquare
  | Disclosure
  | ShadowlessSquare
  | Circular
  | TexturedSquare
  | HelpButton;

[@noalloc] external make: unit => t = "ml_BriskButton_make";

[@noalloc] external setTitle: (t, string) => unit = "ml_BriskButton_setTitle";
[@noalloc]
external setCallback: (t, unit => unit) => unit = "ml_BriskButton_setCallback";

[@noalloc]
external setButtonType: (t, buttonType) => unit =
  "ml_BriskButton_setButtonType_bc" "ml_BriskButton_setButtonType";

[@noalloc]
external setBezelStyle: (t, bezelStyle) => unit =
  "ml_BriskButton_setBezelStyle_bc" "ml_BriskButton_setBezelStyle";

[@noalloc]
external setIsBordered: (t, [@untagged] int) => unit =
  "ml_BriskButton_setIsBordered_bc" "ml_BriskButton_setIsBordered";

let setIsBordered = (btn, bordered) => {
  setIsBordered(btn, bordered ? 1 : 0);
};

let make = (~type_=?, ~bezel=?, ~title=?, ~onClick=?, ()) => {
  let btn = make();

  switch (type_) {
  | Some(buttonType) => setButtonType(btn, buttonType)
  | None => ()
  };

  switch (bezel) {
  | Some(bezelStyle) => setBezelStyle(btn, bezelStyle)
  | None => ()
  };

  switch (title) {
  | Some(str) => setTitle(btn, str)
  | None => ()
  };

  switch (onClick) {
  | Some(callback) => setCallback(btn, callback)
  | None => ()
  };

  btn;
};
