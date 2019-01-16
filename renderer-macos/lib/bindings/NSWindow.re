[@noalloc]
external _NSWindow_makeWithContentRect:
  ([@unboxed] float, [@unboxed] float, [@unboxed] float, [@unboxed] float) =>
  CocoaClass.window =
  "ml_NSWindow_makeWithContentRect_bc" "ml_NSWindow_makeWithContentRect";

[@noalloc]
external _NSWindow_isVisible: CocoaClass.window => bool =
  "ml_NSWindow_isVisible";
[@noalloc]
external _NSWindow_center: CocoaClass.window => unit = "ml_NSWindow_center";
[@noalloc]
external _NSWindow_makeKeyAndOrderFront: CocoaClass.window => unit =
  "ml_NSWindow_makeKeyAndOrderFront";
[@noalloc]
external _NSWindow_setTitle: (CocoaClass.window, string) => unit =
  "ml_NSWindow_setTitle";
external _NSWindow_title: CocoaClass.window => string = "ml_NSWindow_title";
[@noalloc]
external _NSWindow_contentView: CocoaClass.window => CocoaClass.view =
  "ml_NSWindow_contentView";
[@noalloc]
external _NSWindow_setContentView: (CocoaClass.window, CocoaClass.view) => unit =
  "ml_NSWindow_setContentView";
[@noalloc]
external _NSWindow_contentWidth: CocoaClass.window => [@unboxed] float =
  "ml_NSWindow_contentWidth" "ml_NSWindow_contentWidth";
[@noalloc]
external _NSWindow_contentHeight: CocoaClass.window => [@unboxed] float =
  "ml_NSWindow_contentHeight" "ml_NSWindow_contentHeight";
external setOnWindowDidResize: (CocoaClass.window, unit => unit) => unit =
  "ml_NSWindow_setOnWindowDidResize";

class type t = {
  pub isVisible: bool;
  pub center: unit;
  pub makeKeyAndOrderFront: unit;
  pub setTitle: string => unit;
  pub title: string;
  pub contentView: CocoaClass.view;
  pub setContentView: CocoaClass.view => unit;
  pub contentWidth: unit => float;
  pub contentHeight: unit => float;
  pub windowDidResize: (unit => unit) => unit;
};

let makeWithContentRect = (x, y, w, h) => {
  let win = _NSWindow_makeWithContentRect(x, y, w, h);

  {
    as _;
    pub isVisible = _NSWindow_isVisible(win);
    pub center = _NSWindow_center(win);
    pub makeKeyAndOrderFront = _NSWindow_makeKeyAndOrderFront(win);
    pub setTitle = s => _NSWindow_setTitle(win, s);
    pub title = _NSWindow_title(win);
    pub contentView = _NSWindow_contentView(win);
    pub setContentView = v => _NSWindow_setContentView(win, v);
    pub contentWidth = _NSWindow_contentWidth(win);
    pub contentHeight = _NSWindow_contentHeight(win);
    pub windowDidResize = f => setOnWindowDidResize(win, f)
  };
};
