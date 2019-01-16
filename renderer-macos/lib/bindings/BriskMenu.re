type nsMenu;
type nsMenuItem;

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

external _NSApplication_setMenu: (nsMenu, kind) => unit =
  "ml_NSApplication_setMenu";

external _NSMenu_make: string => nsMenu = "ml_NSMenu_make";
external _NSMenu_addItem: (nsMenu, string) => nsMenuItem = "ml_NSMenu_addItem";
external _NSMenuItem_setTarget: (nsMenuItem, target) => unit =
  "ml_NSMenuItem_setTarget";
external _NSMenuItem_setAction: (nsMenuItem, action) => unit =
  "ml_NSMenuItem_setAction";
external _NSMenuItem_setKeyEquivalent: (nsMenuItem, string, bool) => unit =
  "ml_NSMenuItem_setKeyEquivalent";

[@noalloc]
external _NSMenu_removeItem: (nsMenu, nsMenuItem) => unit =
  "ml_NSMenu_removeItem";
[@noalloc]
external _NSMenu_addSeparatorItem: nsMenu => unit =
  "ml_NSMenu_addSeparatorItem";
[@noalloc]
external _NSMenu_setSubmenu: (nsMenu, nsMenu, nsMenuItem) => unit =
  "ml_NSMenu_setSubmenu";

class type t = {
  pub get: unit => nsMenu;
  pub addItem:
    (~action: action=?, ~target: target=?, string, string) => nsMenuItem;
  pub removeItem: nsMenuItem => unit;
  pub addSeparatorItem: unit => unit;
  pub setSubmenu: (nsMenuItem, t) => unit;
};

let make = title => {
  let menu = _NSMenu_make(title);

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

let add = (~kind, menu) => _NSApplication_setMenu(menu#get, kind);
