/**
   Cocoa bindings for OCaml.
   Original project by Nicolás Ojeda Bär © https://github.com/nojb/ocaml-cocoa
*/
module CocoaClass = {
  type window;
  type view;
  type application;
};

module NSApplication = {
  type t = CocoaClass.application;

  external init: unit => unit = "ml_NSApplication_configure";

  external main: unit => unit = "ml_NSApplication_main";

  let isInitialized = ref(false);
  let init = () =>
    if (isInitialized^ === false) {
      init();
      isInitialized := true;
    };

  let identity_f = () => ();

  exception NSAppNotInitialized;

  let willTerminate = func => {
    if (! isInitialized^) {
      raise(NSAppNotInitialized);
    };
    Callback.register("NSAppDelegate.applicationWillTerminate", func);
  };

  let willFinishLaunching = func => {
    if (! isInitialized^) {
      raise(NSAppNotInitialized);
    };
    Callback.register("NSAppDelegate.applicationWillFinishLaunching", func);
  };

  let didFinishLaunching = func => {
    if (! isInitialized^) {
      raise(NSAppNotInitialized);
    };
    Callback.register("NSAppDelegate.applicationDidFinishLaunching", func);
  };
};

module NSWindow = {
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
  external _NSWindow_setContentView:
    (CocoaClass.window, CocoaClass.view) => unit =
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
};

module NSView = {
  type t = CocoaClass.view;

  [@noalloc] external make: unit => t = "ml_NSView_make";
  [@noalloc] external addSubview: (t, t) => unit = "ml_NSView_addSubview";
  [@noalloc] external removeSubview: t => unit = "ml_NSView_removeSubview";

  [@noalloc]
  external setFrame:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setFrame_bc" "ml_NSView_setFrame";

  [@noalloc]
  external setBorderWidth: (CocoaClass.view, [@unboxed] float) => unit =
    "ml_NSView_setBorderWidth_bc" "ml_NSView_setBorderWidth";

  [@noalloc]
  external setBorderColor:
    (
      CocoaClass.view,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setBorderColor_bc" "ml_NSView_setBorderColor";

  [@noalloc]
  external setBackgroundColor:
    (
      CocoaClass.view,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setBackgroundColor_bc" "ml_NSView_setBackgroundColor";
};

module NSTextView = {
  type t = NSView.t;

  [@noalloc] external make: unit => t = "ml_NSTextView_make";

  [@noalloc]
  external getTextWidth: t => [@unboxed] float =
    "ml_NSTextView_getTextWidth_bc" "ml_NSTextView_getTextWidth";

  [@noalloc]
  external getTextHeight: t => [@unboxed] float =
    "ml_NSTextView_getTextHeight_bc" "ml_NSTextView_getTextHeight";

  [@noalloc]
  external setStringValue: (t, string) => unit =
    "ml_NSTextView_setStringValue";

  [@noalloc]
  external setFont: (t, string, [@unboxed] float) => unit =
    "ml_NSTextView_setFont" "ml_NSTextView_setFont";

  [@noalloc]
  external setKern: (t, [@unboxed] float) => unit =
    "ml_NSTextView_setKern_bc" "ml_NSTextView_setKern";

  [@noalloc]
  external setColor:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSTextView_setColor_bc" "ml_NSTextView_setColor";

  [@noalloc]
  external setAlignment: (t, [@untagged] int) => unit =
    "ml_NSTextView_setAlignment_bc" "ml_NSTextView_setAlignment";

  [@noalloc]
  external setLineBreak: (t, [@untagged] int) => unit =
    "ml_NSTextView_setLineBreakMode_bc" "ml_NSTextView_setLineBreakMode";

  [@noalloc]
  external setLineSpacing: (t, [@unboxed] float) => unit =
    "ml_NSTextView_setLineSpacing_bc" "ml_NSTextView_setLineSpacing";

  [@noalloc]
  external setBackgroundColor:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSTextView_setBackgroundColor_bc" "ml_NSTextView_setBackgroundColor";

  let make = str => {
    let txt = make();

    setStringValue(txt, str);
    txt;
  };
};

module NSButton = {
  type t = NSView.t;

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

  [@noalloc] external make: unit => t = "ml_NSButton_make";

  [@noalloc] external setTitle: (t, string) => unit = "ml_NSButton_setTitle";
  [@noalloc]
  external setCallback: (t, unit => unit) => unit = "ml_NSButton_setCallback";

  [@noalloc]
  external setButtonType: (t, buttonType) => unit =
    "ml_NSButton_setButtonType_bc" "ml_NSButton_setButtonType";

  [@noalloc]
  external setBezelStyle: (t, bezelStyle) => unit =
    "ml_NSButton_setBezelStyle_bc" "ml_NSButton_setBezelStyle";

  [@noalloc]
  external setIsBordered: (t, bool) => unit =
    "ml_NSButton_setIsBordered_bc" "ml_NSButton_setIsBordered";

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
};
