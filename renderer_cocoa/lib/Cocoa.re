/**
   Cocoa bindings for OCaml.
   Original project by Nicolás Ojeda Bär © https://github.com/nojb/ocaml-cocoa
*/
module T =
  Ephemeron.K1.Make({
    type t = int;

    let equal = (==);
    let hash = Hashtbl.hash;
  });

external _NSLog: string => unit = "ml_NSLog";

let log = fmt => Printf.ksprintf(_NSLog, fmt);

type nsApp;

type nsWindow;

type nsMenu;
type nsMenuItem;

type nsView;

module NSMenu = {
  type item;

  type kind =
    | Apple
    | Main
    | Services
    | Windows;

  type action =
    /* Apple */
    | OrderFrontStandardAboutPanel
    | Hide
    | HideOtherApplications
    | UnhideAllApplications
    | Terminate
    /* File */
    | Close
    | RunPageLayout
    | Print
    /* Edit */
    | Undo
    | Redo
    | Cut
    | Copy
    | Paste
    | PasteAsPlainText
    | Delete
    | SelectAll
    /* Find */
    | Find
    | FindNext
    | FindPrevious
    | FindUseSelection
    | JumpToSelection
    /* Spelling */
    | ShowGuessPanel
    | CheckSpelling
    | CheckSpellingContinuously
    /* Window */
    | Minimize
    | Zoom
    | BringAllInFront
    /* Help */
    | Help
    /* Custom callback */
    | Callback(unit => unit);

  type target =
    | NSApp;

  external _NSMenu_make: string => nsMenu = "ml_NSMenu_make";
  external _NSMenu_addItem: (nsMenu, string) => item = "ml_NSMenu_addItem";
  external _NSMenuItem_setTarget: (item, target) => unit =
    "ml_NSMenuItem_setTarget";
  external _NSMenuItem_setAction: (item, action) => unit =
    "ml_NSMenuItem_setAction";
  external _NSMenuItem_setKeyEquivalent: (item, string, bool) => unit =
    "ml_NSMenuItem_setKeyEquivalent";

  external _NSMenu_removeItem: (nsMenu, item) => unit = "ml_NSMenu_removeItem";
  external _NSMenu_addSeparatorItem: nsMenu => unit =
    "ml_NSMenu_addSeparatorItem";
  external _NSMenu_setSubmenu: (nsMenu, nsMenu, item) => unit =
    "ml_NSMenu_setSubmenu";

  class type t = {
    pub get: unit => nsMenu;
    pub addItem: (~action: action=?, ~target: target=?, string, string) => item;
    pub removeItem: item => unit;
    pub addSeparatorItem: unit => unit;
    pub setSubmenu: (item, t) => unit;
  };

  let menu_id = ref(0);
  let menu_table = T.create(0);

  let make = title => {
    let id = {
      incr(menu_id);
      menu_id^;
    };

    let menu = _NSMenu_make(title);

    T.add(menu_table, id, menu);

    {
      as _;
      pub get = menu;
      pub addItem =
          (~action=?, ~target=?, ~key="", ~optionModifier=false, title) => {
        let item = _NSMenu_addItem(menu, title);
        switch (action) {
        | Some(action) => _NSMenuItem_setAction(item, action)
        | None => ()
        };
        switch (target) {
        | Some(target) => _NSMenuItem_setTarget(item, target)
        | None => ()
        };
        switch (key) {
        | "" => ()
        | key => _NSMenuItem_setKeyEquivalent(item, key, optionModifier)
        };
        item;
      };
      pub removeItem = item => _NSMenu_removeItem(menu, item);
      pub addSeparatorItem = _NSMenu_addSeparatorItem(menu);
      pub setSubmenu = (item, submenu) =>
        _NSMenu_setSubmenu(menu, submenu#get, item)
    };
  };
};

module NSApplication = {
  external _NSApplication_NSApp: int => nsApp = "ml_NSApplication_NSApp";
  external _NSApplication_run: nsApp => unit = "ml_NSApplication_run";

  external _NSApplication_setMenu: (NSMenu.kind, nsMenu) => unit =
    "ml_NSApplication_setMenu";

  class type t = {
    pub run: unit;
    pub applicationWillFinishLaunching: (unit => unit) => unit;
    pub applicationDidFinishLaunching: (unit => unit) => unit;
  };

  type applicationDelegate = {
    mutable applicationWillFinishLaunching: unit => unit,
    mutable applicationDidFinishLaunching: unit => unit,
  };

  let delegate = () => {
    applicationWillFinishLaunching: () => (),
    applicationDidFinishLaunching: () => (),
  };

  let application_table = T.create(1);
  let app_id = ref(0);

  let app =
    lazy {
      let id = {
        incr(app_id);
        app_id^;
      };
      let app = _NSApplication_NSApp(id);
      let del = delegate();

      T.add(application_table, id, del);

      {
        as _;
        pub run = _NSApplication_run(app);
        pub applicationWillFinishLaunching = f =>
          del.applicationWillFinishLaunching = f;
        pub applicationDidFinishLaunching = f =>
          del.applicationDidFinishLaunching = f
      };
    };

  type delegateSelector =
    | ApplicationWillFinishLaunching
    | ApplicationDidFinishLaunching;

  let applicationDelegate = (id, sel) =>
    switch (T.find(application_table, id)) {
    | exception Not_found =>
      Printf.sprintf("NSApp #%d has been GCed", id) |> failwith
    | del =>
      switch (sel) {
      | ApplicationWillFinishLaunching => del.applicationWillFinishLaunching()
      | ApplicationDidFinishLaunching => del.applicationDidFinishLaunching()
      }
    };

  let setMenu = (~kind, menu) => _NSApplication_setMenu(kind, menu#get);

  let () = Callback.register("NSApp.delegate", applicationDelegate);
};

module NSWindow = {
  [@noalloc]
  external _NSWindow_makeWithContentRect:
    (
      [@untagged] int,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    nsWindow =
    "ml_NSWindow_makeWithContentRect_bc" "ml_NSWindow_makeWithContentRect";

  [@noalloc]
  external _NSWindow_isVisible: nsWindow => bool =
    "ml_NSWindow_isVisible_bc" "ml_NSWindow_isVisible";
  [@noalloc]
  external _NSWindow_center: nsWindow => unit =
    "ml_NSWindow_center_bc" "ml_NSWindow_center";
  [@noalloc]
  external _NSWindow_makeKeyAndOrderFront: nsWindow => unit =
    "ml_NSWindow_makeKeyAndOrderFront_bc" "ml_NSWindow_makeKeyAndOrderFront";
  [@noalloc]
  external _NSWindow_setTitle: (nsWindow, string) => unit =
    "ml_NSWindow_setTitle";
  external _NSWindow_title: nsWindow => string = "ml_NSWindow_title";
  [@noalloc]
  external _NSWindow_contentView: nsWindow => nsView =
    "ml_NSWindow_contentView_bc" "ml_NSWindow_contentView";
  [@noalloc]
  external _NSWindow_setContentView: (nsWindow, nsView) => unit =
    "ml_NSWindow_setContentView_bc" "ml_NSWindow_setContentView";
  [@noalloc]
  external _NSWindow_contentWidth: nsWindow => [@unboxed] float =
    "ml_NSWindow_contentWidth" "ml_NSWindow_contentWidth";
  [@noalloc]
  external _NSWindow_contentHeight: nsWindow => [@unboxed] float =
    "ml_NSWindow_contentHeight" "ml_NSWindow_contentHeight";

  class type t = {
    pub isVisible: bool;
    pub center: unit;
    pub makeKeyAndOrderFront: unit;
    pub setTitle: string => unit;
    pub title: string;
    pub contentView: nsView;
    pub setContentView: nsView => unit;
    pub contentWidth: unit => float;
    pub contentHeight: unit => float;
    pub windowDidResize: (unit => unit) => unit;
  };

  type windowDelegate = {mutable windowDidResize: unit => unit};

  let delegate = () => {windowDidResize: () => ()};

  let win_id = ref(0);
  let window_table = T.create(0);

  let makeWithContentRect = (x, y, w, h) => {
    let id = {
      incr(win_id);
      win_id^;
    };
    let win = _NSWindow_makeWithContentRect(id, x, y, w, h);
    let del = delegate();

    T.add(window_table, id, del);

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
      pub windowDidResize = f => del.windowDidResize = f
    };
  };

  type windowDelegateSelector =
    | WindowDidResize;

  let windowDelegate = (id, sel) =>
    switch (T.find(window_table, id)) {
    | exception Not_found =>
      Printf.sprintf("Window #%d has been GCed", id) |> failwith
    | del =>
      switch (sel) {
      | WindowDidResize => del.windowDidResize()
      }
    };

  let () = Callback.register("NSWindow.delegate", windowDelegate);
};

module NSView = {
  type t = nsView;

  [@noalloc]
  external _NSView_make: unit => t = "ml_NSView_make_bc" "ml_NSView_make";
  [@noalloc]
  external _NSView_memoize: ([@untagged] int, t) => unit =
    "ml_NSView_memoize_bc" "ml_NSView_memoize";

  [@noalloc]
  external _NSView_free: ([@untagged] int) => unit =
    "ml_NSView_free_bc" "ml_NSView_free";

  [@noalloc]
  external _NSView_setBorderWidth: (t, [@unboxed] float) => unit =
    "ml_NSView_setBorderWidth_bc" "ml_NSView_setBorderWidth";

  [@noalloc]
  external _NSView_setBorderColor:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setBorderColor_bc" "ml_NSView_setBorderColor";

  [@noalloc]
  external _NSView_setBackgroundColor:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setBackgroundColor_bc" "ml_NSView_setBackgroundColor";

  [@noalloc]
  external _NSView_addSubview: (t, t) => unit =
    "ml_NSView_addSubview_bc" "ml_NSView_addSubview";
  [@noalloc]
  external _NSView_removeSubview: t => unit =
    "ml_NSView_removeSubview_bc" "ml_NSView_removeSubview";

  [@noalloc]
  external _NSView_setFrame:
    (
      t,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float
    ) =>
    unit =
    "ml_NSView_setFrame_bc" "ml_NSView_setFrame";

  let make = _NSView_make;

  let memoize = _NSView_memoize;
  let free = _NSView_free;

  let addSubview = (view, child, _position) =>
    _NSView_addSubview(view, child);

  let removeSubview = _NSView_removeSubview;

  let setFrame = _NSView_setFrame;

  let setBorderWidth = _NSView_setBorderWidth;
  let setBorderColor = _NSView_setBorderColor;
  let setBackgroundColor = _NSView_setBackgroundColor;
};

module NSButton = {
  type t = NSView.t;

  [@noalloc]
  external _NSButton_make: unit => t =
    "ml_NSButton_make_bc" "ml_NSButton_make";

  [@noalloc]
  external _NSButton_setTitle: (t, string) => unit = "ml_NSButton_setTitle";
  [@noalloc]
  external _NSButton_setCallback: (t, unit => unit) => unit =
    "ml_NSButton_setCallback";

  let make = _NSButton_make;

  let setTitle = _NSButton_setTitle;

  let setCallback = _NSButton_setCallback;
};
