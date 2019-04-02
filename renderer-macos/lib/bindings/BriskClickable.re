type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskClickable_make";

[@noalloc]
external setOnClick: (t, option(unit => unit)) => unit =
  "ml_BriskClickable_setOnClick";

let setOnClick = (btn, callback) => {
  switch (callback) {
  | Some(callback) => setOnClick(btn, Some(UIEventCallback.make(callback)))
  | None => setOnClick(btn, None)
  };
};

external setOnHover: (t, option(bool => unit)) => unit =
  "ml_BriskClickable_setOnHover";

let setOnHover = (btn, callback) => {
  switch (callback) {
  | Some(callback) =>
    setOnHover(
      btn,
      Some(
        mouseOver => UIEventCallback.make(() => callback(mouseOver), ()),
      ),
    )
  | None => setOnHover(btn, None)
  };
};
