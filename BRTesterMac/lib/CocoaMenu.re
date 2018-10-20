/**
   Cocoa NSMenu bindings for OCaml.
*/
module T =
  Ephemeron.K1.Make({
    type t = int;

    let equal = (==);
    let hash = Hashtbl.hash;
  });

type nsMenu;
type nsMenuItem;

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

  external _NSApplication_setMenu: (kind, nsMenu) => unit =
    "ml_NSApplication_setMenu";

  external _NSMenu_make: string => nsMenu = "ml_NSMenu_make";
  external _NSMenu_addItem: (nsMenu, string) => item = "ml_NSMenu_addItem";
  external _NSMenuItem_setTarget: (item, target) => unit =
    "ml_NSMenuItem_setTarget";
  external _NSMenuItem_setAction: (item, action) => unit =
    "ml_NSMenuItem_setAction";
  external _NSMenuItem_setKeyEquivalent: (item, string, bool) => unit =
    "ml_NSMenuItem_setKeyEquivalent";

  [@noalloc]
  external _NSMenu_removeItem: (nsMenu, item) => unit = "ml_NSMenu_removeItem";
  [@noalloc]
  external _NSMenu_addSeparatorItem: nsMenu => unit =
    "ml_NSMenu_addSeparatorItem";
  [@noalloc]
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

  let add = (~kind, menu) => _NSApplication_setMenu(kind, menu#get);
};
