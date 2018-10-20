#import <Cocoa/Cocoa.h>

#define CAML_NAME_SPACE

#import <caml/memory.h>

#define Val_NSMenu(v) ((value)(v))
#define NSMenu_val(v) ((__bridge NSMenu *)(value)(v))

#define Val_NSMenuItem(v) ((value)(v))
#define NSMenuItem_val(v) ((__bridge NSMenuItem *)(value)(v))

enum {
  // Apple
  ActionOrderFrontStandardAboutPanel,
  ActionHide,
  ActionHideOtherApplications,
  ActionUnhideAllApplications,
  ActionTerminate,
  // File
  ActionClose,
  ActionRunPageLayout,
  ActionPrint,
  // Edit
  ActionUndo,
  ActionRedo,
  ActionCut,
  ActionCopy,
  ActionPaste,
  ActionPasteAsPlainText,
  ActionDelete,
  ActionSelectAll,
  // Find
  ActionFind,
  ActionFindNext,
  ActionFindPrevious,
  ActionFindUseSelection,
  ActionJumpToSelection,
  // Spelling
  ActionShowGuessPanel,
  ActionCheckSpelling,
  ActionCheckSpellingContinuously,
  // Window
  ActionMinimize,
  ActionZoom,
  ActionBringAllInFront,
  // Help
  ActionHelp
};

enum {
  TargetNSApp,
};

enum {
  KindApple,
  KindMain,
  KindServices,
  KindWindows,
};

CAMLprim value ml_NSApplication_setMenu(value kind_v, value menu_v) {
  CAMLparam2(kind_v, menu_v);

  NSMenu *menu = NSMenu_val(menu_v);

  switch (Int_val(kind_v)) {
  case KindApple:
    [NSApp performSelector:@selector(setAppleMenu:) withObject:menu];
    break;
  case KindMain:
    [NSApp setMainMenu:menu];
    break;
  case KindServices:
    [NSApp setServicesMenu:menu];
    break;
  case KindWindows:
    [NSApp setWindowsMenu:menu];
    break;
  }

  CAMLreturn(Val_unit);
}

// NSMenu
CAMLprim value ml_NSMenu_make(value title_v) {
  CAMLparam1(title_v);

  NSString *title = [NSString stringWithUTF8String:String_val(title_v)];
  NSMenu *menu = [[NSMenu alloc] initWithTitle:title];

  CAMLreturn(Val_NSMenu(menu));
}

CAMLprim value ml_NSMenu_addItem(value menu_v, value title_v) {
  CAMLparam2(menu_v, title_v);

  NSMenu *menu = NSMenu_val(menu_v);

  NSString *title = [NSString stringWithUTF8String:String_val(title_v)];

  NSMenuItem *item =
      [menu addItemWithTitle:title action:NULL keyEquivalent:@""];

  CAMLreturn(Val_NSMenuItem(item));
}

void ml_NSMenu_removeItem(NSMenu *menu, NSMenuItem *item) {
  [menu removeItem:item];
}

void ml_NSMenu_addSeparatorItem(NSMenu *menu) {
  [menu addItem:[NSMenuItem separatorItem]];
}

CAMLprim value ml_NSMenuItem_setTarget(value item_v, value target_v) {
  CAMLparam2(item_v, target_v);

  NSMenuItem *item = NSMenuItem_val(item_v);

  if (Is_long(target_v)) {
    switch (Int_val(target_v)) {
    case TargetNSApp:
      [item setTarget:NSApp];
      break;
    }
  } else {
    // TODO Window or element target
  }

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenuItem_setAction(value item_v, value action_v) {
  CAMLparam2(item_v, action_v);

  NSMenuItem *item = NSMenuItem_val(item_v);

  SEL action = NULL;

  switch (Int_val(action_v)) {
  case ActionOrderFrontStandardAboutPanel:
    action = @selector(orderFrontStandardAboutPanel:);
    break;
  case ActionHide:
    action = @selector(hide:);
    break;
  case ActionHideOtherApplications:
    action = @selector(hideOtherApplications:);
    break;
  case ActionUnhideAllApplications:
    action = @selector(unhideAllApplications:);
    break;
  case ActionTerminate:
    action = @selector(terminate:);
    break;
  case ActionClose:
    action = @selector(performClose:);
    break;
  case ActionRunPageLayout:
    action = @selector(runPageLayout:);
    break;
  case ActionPrint:
    action = @selector(print:);
    break;
  case ActionUndo:
    action = @selector(undo:);
    break;
  case ActionRedo:
    action = @selector(redo:);
    break;
  case ActionCut:
    action = @selector(cut:);
    break;
  case ActionCopy:
    action = @selector(copy:);
    break;
  case ActionPaste:
    action = @selector(paste:);
    break;
  case ActionPasteAsPlainText:
    action = @selector(pasteAsPlainText:);
    break;
  case ActionDelete:
    action = @selector(delete:);
    break;
  case ActionSelectAll:
    action = @selector(selectAll:);
    break;
  case ActionFind:
    [item setTag:NSFindPanelActionShowFindPanel];
    action = @selector(performFindPanelAction:);
    break;
  case ActionFindNext:
    [item setTag:NSFindPanelActionNext];
    action = @selector(performFindPanelAction:);
    break;
  case ActionFindPrevious:
    [item setTag:NSFindPanelActionPrevious];
    action = @selector(performFindPanelAction:);
    break;
  case ActionFindUseSelection:
    [item setTag:NSFindPanelActionSetFindString];
    action = @selector(performFindPanelAction:);
    break;
  case ActionJumpToSelection:
    action = @selector(centerSelectionInVisibleArea:);
    break;
  case ActionShowGuessPanel:
    action = @selector(showGuessPanel:);
    break;
  case ActionCheckSpelling:
    action = @selector(checkSpelling:);
    break;
  case ActionCheckSpellingContinuously:
    action = @selector(toggleContinuousSpellChecking:);
    break;

  case ActionMinimize:
    action = @selector(performMinimize:);
    break;
  case ActionZoom:
    action = @selector(performZoom:);
    break;
  case ActionBringAllInFront:
    action = @selector(arrangeInFront:);
    break;
  case ActionHelp:
    action = @selector(showHelp:);
    break;
  }

  [item setAction:action];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenuItem_setKeyEquivalent(value item_v, value key_v,
                                              value option_modifier_v) {
  CAMLparam3(item_v, key_v, option_modifier_v);

  NSMenuItem *item = NSMenuItem_val(item_v);
  NSString *key = [NSString stringWithUTF8String:String_val(key_v)];

  [item setKeyEquivalent:key];

  if (Bool_val(option_modifier_v)) {
    [item setKeyEquivalentModifierMask:NSEventModifierFlagCommand |
                                       NSEventModifierFlagOption];
  }

  CAMLreturn(Val_unit);
}

void ml_NSMenu_setSubmenu(NSMenu *menu, NSMenu *submenu, NSMenuItem *item) {
  [menu setSubmenu:submenu forItem:item];
}
