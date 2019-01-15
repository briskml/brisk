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

module BriskTextView = {
  type t = NSView.t;

  [@noalloc] external make: unit => t = "ml_BriskTextView_make";

  [@noalloc]
  external getTextWidth: t => [@unboxed] float =
    "ml_BriskTextView_getTextWidth_bc" "ml_BriskTextView_getTextWidth";

  [@noalloc]
  external getTextHeight: t => [@unboxed] float =
    "ml_BriskTextView_getTextHeight_bc" "ml_BriskTextView_getTextHeight";

  [@noalloc]
  external setStringValue: (t, string) => unit =
    "ml_BriskTextView_setStringValue";

  [@noalloc]
  external setFont: (t, string, [@unboxed] float, [@unboxed] float) => unit =
    "ml_BriskTextView_setFont" "ml_BriskTextView_setFont";

  [@noalloc]
  external setKern: (t, [@unboxed] float) => unit =
    "ml_BriskTextView_setKern_bc" "ml_BriskTextView_setKern";

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
    "ml_BriskTextView_setColor_bc" "ml_BriskTextView_setColor";

  [@noalloc]
  external setAlignment: (t, [@untagged] int) => unit =
    "ml_BriskTextView_setAlignment_bc" "ml_BriskTextView_setAlignment";

  [@noalloc]
  external setLineBreak: (t, [@untagged] int) => unit =
    "ml_BriskTextView_setLineBreakMode_bc" "ml_BriskTextView_setLineBreakMode";

  [@noalloc]
  external setLineSpacing: (t, [@unboxed] float) => unit =
    "ml_BriskTextView_setLineSpacing_bc" "ml_BriskTextView_setLineSpacing";

  [@noalloc]
  external setCornerRadius: (t, [@unboxed] float) => unit =
    "ml_BriskTextView_setCornerRadius_bc" "ml_BriskTextView_setCornerRadius";

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
    "ml_BriskTextView_setBackgroundColor_bc"
    "ml_BriskTextView_setBackgroundColor";

  [@noalloc]
  external setPadding:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_BriskTextView_setPadding_bc" "ml_BriskTextView_setPadding";

  let make = str => {
    let txt = make();

    setStringValue(txt, str);
    txt;
  };
};

module BriskImage = {
  type t = NSView.t;

  type named = [
    | `ImageActionTemplate
    | `ImageAddTemplate
    | `ImageAdvanced
    | `ImageApplicationIcon
    | `ImageBluetoothTemplate
    | `ImageBonjour
    | `ImageBookmarksTemplate
    | `ImageCaution
    | `ImageColorPanel
    | `ImageColumnViewTemplate
    | `ImageComputer
    | `ImageEnterFullScreenTemplate
    | `ImageEveryone
    | `ImageExitFullScreenTemplate
    | `ImageFlowViewTemplate
    | `ImageFolder
    | `ImageFolderBurnable
    | `ImageFolderSmart
    | `ImageFollowLinkFreestandingTemplate
    | `ImageFontPanel
    | `ImageGoLeftTemplate
    | `ImageGoRightTemplate
    | `ImageHomeTemplate
    | `ImageIChatTheaterTemplate
    | `ImageIconViewTemplate
    | `ImageInfo
    | `ImageInvalidDataFreestandingTemplate
    | `ImageLeftFacingTriangleTemplate
    | `ImageListViewTemplate
    | `ImageLockLockedTemplate
    | `ImageLockUnlockedTemplate
    | `ImageMenuMixedStateTemplate
    | `ImageMenuOnStateTemplate
    | `ImageMobileMe
    | `ImageMultipleDocuments
    | `ImageNetwork
    | `ImagePathTemplate
    | `ImagePreferencesGeneral
    | `ImageQuickLookTemplate
    | `ImageRefreshFreestandingTemplate
    | `ImageRefreshTemplate
    | `ImageRemoveTemplate
    | `ImageRevealFreestandingTemplate
    | `ImageRightFacingTriangleTemplate
    | `ImageShareTemplate
    | `ImageSlideshowTemplate
    | `ImageSmartBadgeTemplate
    | `ImageStatusAvailable
    | `ImageStatusNone
    | `ImageStatusPartiallyAvailable
    | `ImageStatusUnavailable
    | `ImageStopProgressFreestandingTemplate
    | `ImageStopProgressTemplate
    | `ImageTrashEmpty
    | `ImageTrashFull
    | `ImageUser
    | `ImageUserAccounts
    | `ImageUserGroup
    | `ImageUserGuest
  ];

  type source = [ | `file(string) | `named(string) | `bundle(string)];

  let stringOfNamed =
    fun
    | `ImageActionTemplate => "NSActionTemplate"
    | `ImageAddTemplate => "NSAddTemplate"
    | `ImageAdvanced => "NSAdvanced"
    | `ImageApplicationIcon => "NSApplicationIcon"
    | `ImageBluetoothTemplate => "NSBluetoothTemplate"
    | `ImageBonjour => "NSBonjour"
    | `ImageBookmarksTemplate => "NSBookmarksTemplate"
    | `ImageCaution => "NSCaution"
    | `ImageColorPanel => "NSColorPanel"
    | `ImageColumnViewTemplate => "NSColumnViewTemplate"
    | `ImageComputer => "NSComputer"
    | `ImageEnterFullScreenTemplate => "NSEnterFullScreenTemplate"
    | `ImageEveryone => "NSEveryone"
    | `ImageExitFullScreenTemplate => "NSExitFullScreenTemplate"
    | `ImageFlowViewTemplate => "NSFlowViewTemplate"
    | `ImageFolder => "NSFolder"
    | `ImageFolderBurnable => "NSFolderBurnable"
    | `ImageFolderSmart => "NSFolderSmart"
    | `ImageFollowLinkFreestandingTemplate => "NSFollowLinkFreestandingTemplate"
    | `ImageFontPanel => "NSFontPanel"
    | `ImageGoLeftTemplate => "NSGoLeftTemplate"
    | `ImageGoRightTemplate => "NSGoRightTemplate"
    | `ImageHomeTemplate => "NSHomeTemplate"
    | `ImageIChatTheaterTemplate => "NSIChatTheaterTemplate"
    | `ImageIconViewTemplate => "NSIconViewTemplate"
    | `ImageInfo => "NSInfo"
    | `ImageInvalidDataFreestandingTemplate => "NSInvalidDataFreestandingTemplate"
    | `ImageLeftFacingTriangleTemplate => "NSLeftFacingTriangleTemplate"
    | `ImageListViewTemplate => "NSListViewTemplate"
    | `ImageLockLockedTemplate => "NSLockLockedTemplate"
    | `ImageLockUnlockedTemplate => "NSLockUnlockedTemplate"
    | `ImageMenuMixedStateTemplate => "NSMenuMixedStateTemplate"
    | `ImageMenuOnStateTemplate => "NSMenuOnStateTemplate"
    | `ImageMobileMe => "NSMobileMe"
    | `ImageMultipleDocuments => "NSMultipleDocuments"
    | `ImageNetwork => "NSNetwork"
    | `ImagePathTemplate => "NSPathTemplate"
    | `ImagePreferencesGeneral => "NSPreferencesGeneral"
    | `ImageQuickLookTemplate => "NSQuickLookTemplate"
    | `ImageRefreshFreestandingTemplate => "NSRefreshFreestandingTemplate"
    | `ImageRefreshTemplate => "NSRefreshTemplate"
    | `ImageRemoveTemplate => "NSRemoveTemplate"
    | `ImageRevealFreestandingTemplate => "NSRevealFreestandingTemplate"
    | `ImageRightFacingTriangleTemplate => "NSRightFacingTriangleTemplate"
    | `ImageShareTemplate => "NSShareTemplate"
    | `ImageSlideshowTemplate => "NSSlideshowTemplate"
    | `ImageSmartBadgeTemplate => "NSSmartBadgeTemplate"
    | `ImageStatusAvailable => "NSStatusAvailable"
    | `ImageStatusNone => "NSStatusNone"
    | `ImageStatusPartiallyAvailable => "NSStatusPartiallyAvailable"
    | `ImageStatusUnavailable => "NSStatusUnavailable"
    | `ImageStopProgressFreestandingTemplate => "NSStopProgressFreestandingTemplate"
    | `ImageStopProgressTemplate => "NSStopProgressTemplate"
    | `ImageTrashEmpty => "NSTrashEmpty"
    | `ImageTrashFull => "NSTrashFull"
    | `ImageUser => "NSUser"
    | `ImageUserAccounts => "NSUserAccounts"
    | `ImageUserGroup => "NSUserGroup"
    | `ImageUserGuest => "NSUserGuest";

  [@noalloc] external make: unit => t = "ml_BriskImage_make";

  [@noalloc]
  external getImageWidth: t => [@unboxed] float =
    "ml_BriskImage_getImageWidth_bc" "ml_BriskImage_getImageWidth";

  [@noalloc]
  external getImageHeight: t => [@unboxed] float =
    "ml_BriskImage_getImageHeight_bc" "ml_BriskImage_getImageHeight";

  [@noalloc]
  external setSourceFile: (t, string) => unit = "ml_BriskImage_setSourceFile";

  [@noalloc]
  external setSourceNamed: (t, string) => unit =
    "ml_BriskImage_setSourceNamed";

  let make = (~source=?, ()) => {
    let img = make();

    switch (source) {
    | Some(src) =>
      switch (src) {
      | `file(source) => setSourceFile(img, source)
      | `named(named) => setSourceNamed(img, stringOfNamed(named))
      | `bundle(source) => setSourceNamed(img, source)
      }
    | None => ()
    };

    img;
  };
};

module BriskButton = {
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

  [@noalloc] external make: unit => t = "ml_BriskButton_make";

  [@noalloc]
  external setTitle: (t, string) => unit = "ml_BriskButton_setTitle";
  [@noalloc]
  external setCallback: (t, unit => unit) => unit =
    "ml_BriskButton_setCallback";

  [@noalloc]
  external setButtonType: (t, buttonType) => unit =
    "ml_BriskButton_setButtonType_bc" "ml_BriskButton_setButtonType";

  [@noalloc]
  external setBezelStyle: (t, bezelStyle) => unit =
    "ml_BriskButton_setBezelStyle_bc" "ml_BriskButton_setBezelStyle";

  [@noalloc]
  external setIsBordered: (t, bool) => unit =
    "ml_BriskButton_setIsBordered_bc" "ml_BriskButton_setIsBordered";

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
