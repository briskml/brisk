open CocoaTypes;

[@noalloc]
external _NSWindow_makeWithContentRect:
  ([@unboxed] float, [@unboxed] float, [@unboxed] float, [@unboxed] float) =>
  window =
  "ml_NSWindow_makeWithContentRect_bc" "ml_NSWindow_makeWithContentRect";

[@noalloc]
external _NSWindow_isVisible: window => bool = "ml_NSWindow_isVisible";
[@noalloc] external _NSWindow_center: window => unit = "ml_NSWindow_center";
[@noalloc]
external _NSWindow_makeKeyAndOrderFront: window => unit =
  "ml_NSWindow_makeKeyAndOrderFront";
[@noalloc]
external _NSWindow_setTitle: (window, string) => unit = "ml_NSWindow_setTitle";
external _NSWindow_title: window => string = "ml_NSWindow_title";
[@noalloc]
external _NSWindow_contentView: window => view = "ml_NSWindow_contentView";
[@noalloc]
external _NSWindow_setContentView: (window, view) => unit =
  "ml_NSWindow_setContentView";
[@noalloc]
external _NSWindow_contentWidth: window => [@unboxed] float =
  "ml_NSWindow_contentWidth" "ml_NSWindow_contentWidth";
[@noalloc]
external _NSWindow_contentHeight: window => [@unboxed] float =
  "ml_NSWindow_contentHeight_bc" "ml_NSWindow_contentHeight";
external setOnWindowDidResize: (window, unit => unit) => unit =
  "ml_NSWindow_setOnWindowDidResize";

let makeWithContentRect = (x, y, w, h) => {
  _NSWindow_makeWithContentRect(x, y, w, h);
};

let isVisible = win => _NSWindow_isVisible(win);
let center = win => _NSWindow_center(win);
let makeKeyAndOrderFront = win => _NSWindow_makeKeyAndOrderFront(win);
let setTitle = (win, s) => _NSWindow_setTitle(win, s);
let title = win => _NSWindow_title(win);
let contentView = win => _NSWindow_contentView(win);
let setContentView = (win, v) => _NSWindow_setContentView(win, v);
let contentWidth = win => _NSWindow_contentWidth(win);
let contentHeight = win => _NSWindow_contentHeight(win);
let windowDidResize = (win, f) => {
  setOnWindowDidResize(win, UIEventCallback.make(f));
};
