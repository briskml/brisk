#import "BriskCocoa.h"
#import "OCamlClosureEventTarget.h"

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
  KindMain,
  KindServices,
  KindWindows,
};

CAMLprim value ml_NSApplication_setMenu(value kind_v, NSMenu *menu) {
  CAMLparam1(kind_v);

  switch (Int_val(kind_v)) {
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

  CAMLreturn((value)menu);
}

NSMenu *ml_NSMenu_makeServices() { return [NSApp servicesMenu]; }

NSMenu *ml_NSMenu_makeWindows() { return [NSApp windowsMenu]; }

void ml_NSMenu_addItem(NSMenu *menu, NSMenuItem *item) { [menu addItem:item]; }

void ml_NSMenu_removeItem(NSMenu *menu, NSMenuItem *item) {
  [menu removeItem:item];
}

NSMenuItem *ml_NSMenuItem_makeSeparatorItem() {
  return [NSMenuItem separatorItem];
}

NSMenuItem *ml_NSMenuItem_make() { return [NSMenuItem new]; }

CAMLprim value ml_NSMenuItem_setAction(NSMenuItem *item, value action_v) {
  CAMLparam1(action_v);

  SEL action = NULL;

  if (Is_block(action_v)) {
    /* The action block Callback(callback) is equivalent to Some(callback) when
     * it comes to the memory implementation. Hence we don't need a conversion
     */
    [item brisk_unsafe_setClosureTarget:action_v];
  } else {
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
    [item setTarget:NSApp];
    [item setAction:action];
  };

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenuItem_setKeyEquivalent(NSMenuItem *item, value key_v,
                                              value option_modifier_v) {
  CAMLparam2(key_v, option_modifier_v);

  NSString *key = [NSString stringWithUTF8String:String_val(key_v)];

  [item setKeyEquivalent:key];

  if (Bool_val(option_modifier_v)) {
    [item setKeyEquivalentModifierMask:NSEventModifierFlagCommand |
                                       NSEventModifierFlagOption];
  }

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenuItem_setTitle(NSMenuItem *item, value title_v) {
  CAMLparam1(title_v);

  NSString *key = [NSString stringWithUTF8String:String_val(title_v)];

  [item setTitle:key];

  CAMLreturn(Val_unit);
}

void ml_NSMenuItem_setSubmenu(NSMenuItem *item, NSMenu *submenu) {
  [item setSubmenu:submenu];
}
