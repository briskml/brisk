type item;

type menu;

type action =
  BriskMenu.action =
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

let item:
  (
    ~title: string,
    ~action: action,
    ~optionModifier: bool=?,
    ~key: string=?,
    ~children: list(unit),
    unit
  ) =>
  item;

let separatorItem: (~children: list(unit), unit) => item;

let submenu: (~children: menu, unit) => item;

let menu: (~children: list(item), ~title: string, unit) => menu;

let setMainMenu: list(menu) => unit;

let setWindowsMenu: menu => unit;

let setServicesMenu: menu => unit;
