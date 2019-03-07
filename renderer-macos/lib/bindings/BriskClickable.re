type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskClickable_make";

[@noalloc]
external setOnClick: (t, unit => unit) => unit =
  "ml_BriskClickable_setOnClick";

let make = (~onClick=?, ()) => {
  let btn = make();

  switch (onClick) {
  | Some(callback) => setOnClick(btn, UIEventCallback.make(callback))
  | None => ()
  };

  btn;
};
