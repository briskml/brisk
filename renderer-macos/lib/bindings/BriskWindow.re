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

[@noalloc]
external setContentIsFullSize: (window, [@untagged] int) => unit =
  "ml_NSWindow_setContentIsFullSize_bc" "ml_NSWindow_setContentIsFullSize";
let setContentIsFullSize = (win, isFullSize) =>
  setContentIsFullSize(win, isFullSize ? 1 : 0);

[@noalloc]
external setTitleIsHidden: (window, [@untagged] int) => unit =
  "ml_NSWindow_setTitleIsHidden_bc" "ml_NSWindow_setTitleIsHidden";
let setTitleIsHidden = (win, isHidden) =>
  setTitleIsHidden(win, isHidden ? 1 : 0);

[@noalloc]
external setTitlebarIsTransparent: (window, [@untagged] int) => unit =
  "ml_NSWindow_setTitlebarIsTransparent_bc"
  "ml_NSWindow_setTitlebarIsTransparent";
let setTitlebarIsTransparent = (win, isTransparent) =>
  setTitlebarIsTransparent(win, isTransparent ? 1 : 0);

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
external setToolbar: (window, BriskToolbar.t) => unit =
  "ml_NSWindow_setToolbar_bc" "ml_NSWindow_setToolbar";

let make =
    (
      ~width as w,
      ~height as h,
      ~title=?,
      ~contentView=?,
      ~contentIsFullSize=?,
      ~onResize=?,
      (),
    ) => {
  let win = makeWithContentRect(0., 0., w, h);

  center(win);
  makeKeyAndOrderFront(win);

  switch (title) {
  | Some(title) => setTitle(win, title)
  | None => ()
  };

  switch (contentView) {
  | Some(view) => setContentView(win, view)
  | None => ()
  };

  let isFullSize =
    switch (contentIsFullSize) {
    | Some(isFullSize) => isFullSize
    | None => false
    };

  setContentIsFullSize(win, isFullSize);

  switch (onResize) {
  | Some(f) => setOnWindowDidResize(win, UIEventCallback.make(_ => f(win)))
  | None => ()
  };

  win;
};
