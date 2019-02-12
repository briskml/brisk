open CocoaTypes;

[@noalloc]
external makeWithContentRect:
  ([@unboxed] float, [@unboxed] float, [@unboxed] float, [@unboxed] float) =>
  window =
  "ml_NSWindow_makeWithContentRect_bc" "ml_NSWindow_makeWithContentRect";

[@noalloc] external isVisible: window => bool = "ml_NSWindow_isVisible";

[@noalloc] external center: window => unit = "ml_NSWindow_center";

[@noalloc]
external makeKeyAndOrderFront: window => unit =
  "ml_NSWindow_makeKeyAndOrderFront";
[@noalloc]
external setTitle: (window, string) => unit = "ml_NSWindow_setTitle";
external title: window => string = "ml_NSWindow_title";
[@noalloc] external contentView: window => view = "ml_NSWindow_contentView";
[@noalloc]
external setContentView: (window, view) => unit = "ml_NSWindow_setContentView";
[@noalloc]
external contentWidth: window => [@unboxed] float =
  "ml_NSWindow_contentWidth" "ml_NSWindow_contentWidth";
[@noalloc]
external contentHeight: window => [@unboxed] float =
  "ml_NSWindow_contentHeight_bc" "ml_NSWindow_contentHeight";
external setOnWindowDidResize: (window, unit => unit) => unit =
  "ml_NSWindow_setOnWindowDidResize";

let make =
    (~width as w, ~height as h, ~title=?, ~onResize=?, ~contentView=?, ()) => {
  let win = makeWithContentRect(0., 0., w, h);

  center(win);
  makeKeyAndOrderFront(win);

  switch (title) {
  | Some(title) => setTitle(win, title)
  | None => ()
  };

  switch (onResize) {
  | Some(f) => setOnWindowDidResize(win, UIEventCallback.make(_ => f(win)))
  | None => ()
  };

  let contentView =
    switch (contentView) {
    | Some(view) => view
    | None => BriskView.make()
    };

  setContentView(win, contentView);

  win;
};
