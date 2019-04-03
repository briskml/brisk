module Menu = BriskMenu;

type actionItem = {
  action: Menu.action,
  title: string,
  key: option(string),
  optionModifier: bool,
};

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

type item =
  | ActionItem(actionItem)
  | SeparatorItem
  | Submenu(menu)
and menu = {
  title: string,
  children: list(item),
};

let item =
    (
      ~title,
      ~action,
      ~optionModifier=false,
      ~key=?,
      ~children as _: list(unit),
      (),
    ) =>
  ActionItem({action, title, key, optionModifier});

let separatorItem = (~children as _: list(unit), ()) => SeparatorItem;

let submenu = (~children: menu, ()) => Submenu(children);

let menu = (~children, ~title, ()) => {title, children};

let menuTitle = ({title, _}) => title;

type nativeMenu = {
  nativeObject: Menu.t,
  menu,
};

let rec toNativeMenu = ({title, children: items} as menu) => {
  let (nativeMenu, items) = (Menu.make(title), items);
  List.map(toNativeItem, items)
  |> List.iter(item => Menu.addItem(nativeMenu, item));
  {nativeObject: nativeMenu, menu};
}
and toNativeItem = item =>
  switch (item) {
  | ActionItem({action, title, key, optionModifier}) =>
    let item = Menu.Item.make();
    Menu.Item.setAction(item, action);
    Menu.Item.setTitle(item, title);
    switch (key) {
    | Some(key) => Menu.Item.setKeyEquivalent(item, key, optionModifier)
    | None => ()
    };
    item;
  | SeparatorItem => Menu.Item.makeSeparatorItem()
  | Submenu(menu) =>
    let submenu = toNativeMenu(menu);
    let item = Menu.Item.make();
    Menu.Item.setTitle(item, menuTitle(submenu.menu));
    Menu.Item.setSubmenu(item, submenu.nativeObject);
    item;
  };

let setServicesMenu = menu => {
  let {nativeObject: nativeMenu, _} = toNativeMenu(menu);
  Menu.setMenu(Menu.Services, nativeMenu);
}

let setWindowsMenu = menu => {
  let {nativeObject: nativeMenu, _} = toNativeMenu(menu);
  Menu.setMenu(Menu.Windows, nativeMenu);
}

let setMainMenu = menuList => {
  let mainMenu = Menu.make("Main");
  List.map(toNativeMenu, menuList)
  |> List.iter(({nativeObject, menu}) => {
       let item = Menu.Item.make();
       Menu.Item.setTitle(item, menuTitle(menu));
       Menu.Item.setSubmenu(item, nativeObject);
       Menu.addItem(mainMenu, item);
     });
  Menu.setMenu(Menu.Main, mainMenu);
};
