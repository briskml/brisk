open CocoaTypes;

type t = menu;

type kind =
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

external setMenu: (kind, t) => unit = "ml_NSApplication_setMenu";

module Item = {
  type t = menuItem;
  external make: unit => t = "ml_NSMenuItem_make";
  external makeSeparatorItem: unit => t = "ml_NSMenuItem_makeSeparatorItem";
  external setTitle: (t, string) => unit = "ml_NSMenuItem_setTitle";
  external setAction: (t, action) => unit = "ml_NSMenuItem_setAction";
  external setKeyEquivalent: (t, string, bool) => unit =
    "ml_NSMenuItem_setKeyEquivalent";
  external setSubmenu: (menuItem, menu) => unit = "ml_NSMenuItem_setSubmenu";
};

external make: string => menu = "ml_NSMenu_make";
external makeServicesMenu: unit => menu = "ml_NSMenu_makeServices";
external makeWindowsMenu: unit => menu = "ml_NSMenu_makeWindows";
external addItem: (menu, menuItem) => unit = "ml_NSMenu_addItem";
