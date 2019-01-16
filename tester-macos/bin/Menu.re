open Brisk_macos;
open Cocoa;

let makeApplicationMenu = appName => {
  let submenu = BriskMenu.make("Apple");
  BriskMenu.add(~kind=Apple, submenu);

  let _ =
    submenu#addItem(
      ~action=OrderFrontStandardAboutPanel,
      ~target=NSApp,
      "About " ++ appName,
    );
  let _ = submenu#addSeparatorItem;

  let _ = submenu#addItem(~key=",", "Preferences...");
  let _ = submenu#addSeparatorItem;

  let services = submenu#addItem("Services");
  let servicesMenu = BriskMenu.make("Services");
  submenu#setSubmenu(services, servicesMenu);
  BriskMenu.add(~kind=Services, servicesMenu);
  let _ = submenu#addSeparatorItem;

  let _ =
    submenu#addItem(
      ~action=Hide,
      ~target=NSApp,
      ~key="h",
      "Hide " ++ appName,
    );

  let _ =
    submenu#addItem(
      ~action=HideOtherApplications,
      ~target=NSApp,
      ~key="h",
      ~optionModifier=true,
      "Hide Others",
    );

  let _ =
    submenu#addItem(~action=UnhideAllApplications, ~target=NSApp, "Show All");
  let _ = submenu#addSeparatorItem;

  let _ =
    submenu#addItem(
      ~action=Terminate,
      ~target=NSApp,
      ~key="q",
      "Quit " ++ appName,
    );
  submenu;
};

let makeFileMenu = () => {
  let submenu = BriskMenu.make("File");

  let _ = submenu#addItem(~key="n", "New");
  let _ = submenu#addItem(~key="o", "Open...");

  let _ = submenu#addSeparatorItem;

  let _ = submenu#addItem(~action=Close, ~key="w", "Close");

  let _ = submenu#addItem(~key="s", "Save");
  let _ = submenu#addItem(~key="S", "Save as...");
  let _ = submenu#addItem("Revert");

  let _ = submenu#addSeparatorItem;

  let _ = submenu#addItem(~action=RunPageLayout, ~key="P", "Page Setup...");
  let _ = submenu#addItem(~action=Print, ~key="p", "Print...");

  submenu;
};

let makeFindMenu = () => {
  let submenu = BriskMenu.make("Find");

  let _ = submenu#addItem(~action=Find, ~key="f", "Find...");
  let _ = submenu#addItem(~action=FindNext, ~key="g", "Find Next");
  let _ = submenu#addItem(~action=FindPrevious, ~key="G", "Find Previous");
  let _ =
    submenu#addItem(
      ~action=FindUseSelection,
      ~key="e",
      "Use Selection for Find",
    );
  let _ =
    submenu#addItem(~action=JumpToSelection, ~key="j", "Jump to Selection");

  submenu;
};

let makeSpellingMenu = () => {
  let submenu = BriskMenu.make("Spelling");

  let _ = submenu#addItem(~action=ShowGuessPanel, ~key=":", "Spelling...");
  let _ = submenu#addItem(~action=CheckSpelling, ~key=";", "Check Spelling");
  let _ =
    submenu#addItem(
      ~action=CheckSpellingContinuously,
      "Check Spelling as You Type",
    );

  submenu;
};

let makeEditMenu = () => {
  let submenu = BriskMenu.make("Edit");

  let _ = submenu#addItem(~action=Undo, ~key="z", "Undo");
  let _ = submenu#addItem(~action=Redo, ~key="Z", "Redo");
  let _ = submenu#addSeparatorItem;
  let _ = submenu#addItem(~action=Cut, ~key="x", "Cut");
  let _ = submenu#addItem(~action=Copy, ~key="c", "Copy");
  let _ = submenu#addItem(~action=Paste, ~key="v", "Paste");
  let _ =
    submenu#addItem(
      ~action=PasteAsPlainText,
      ~key="V",
      ~optionModifier=true,
      "Paste and Match Style",
    );
  let _ = submenu#addItem(~action=Delete, "Delete");
  let _ = submenu#addItem(~action=SelectAll, ~key="a", "Select All");

  let _ = submenu#addSeparatorItem;

  let find = submenu#addItem("Find");
  let findMenu = makeFindMenu();

  submenu#setSubmenu(find, findMenu);

  let spelling = submenu#addItem("Spelling");
  let spellingMenu = makeSpellingMenu();

  submenu#setSubmenu(spelling, spellingMenu);

  submenu;
};

let makeWindowMenu = () => {
  let submenu = BriskMenu.make("Window");
  BriskMenu.add(~kind=Windows, submenu);

  let _ = submenu#addItem(~action=Minimize, ~key="m", "Minimize");
  let _ = submenu#addItem(~action=Zoom, "Zoom");
  let _ = submenu#addSeparatorItem;
  let _ = submenu#addItem(~action=BringAllInFront, "Bring All In Front");

  submenu;
};

let makeHelpMenu = appName => {
  let submenu = BriskMenu.make("Help");

  let _ =
    submenu#addItem(
      ~action=Help,
      ~target=NSApp,
      ~key="?",
      appName ++ " Help",
    );

  submenu;
};

let makeMainMenu = appName => {
  let mainMenu = BriskMenu.make("MainMenu");

  let apple = mainMenu#addItem("Apple");
  let submenu = makeApplicationMenu(appName);
  mainMenu#setSubmenu(apple, submenu);

  let file = mainMenu#addItem("File");
  let submenu = makeFileMenu();
  mainMenu#setSubmenu(file, submenu);

  let edit = mainMenu#addItem("Edit");
  let submenu = makeEditMenu();
  mainMenu#setSubmenu(edit, submenu);

  let window = mainMenu#addItem("Window");
  let submenu = makeWindowMenu();
  mainMenu#setSubmenu(window, submenu);

  let help = mainMenu#addItem("Help");
  let submenu = makeHelpMenu(appName);
  mainMenu#setSubmenu(help, submenu);

  mainMenu;
};
