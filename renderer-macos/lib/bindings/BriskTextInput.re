type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskTextInput_make";

[@noalloc]
external setCallback: (t, unit => unit) => unit = "ml_BriskTextInput_setCallback";

let make = (~onChange=?, ()) => {
  let input = make();

  switch (onChange) {
  | Some(callback) =>
    setCallback(
      input,
      UIEventCallback.make(callback),
    )
  | None => ()
  };

  input;
};
