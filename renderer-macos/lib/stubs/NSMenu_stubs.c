#import "NSMenu_stubs.h"
#import <Cocoa/Cocoa.h>
#define CAML_NAME_SPACE
#import <caml/mlvalues.h>
#import <caml/memory.h>
#import <caml/alloc.h>
#import <caml/callback.h>

CAMLprim value ml_NSApplication_setMenu(NSMenu *menu, value kind_v) {
  CAMLparam1(kind_v);

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

  CAMLreturn((value)menu);
}

CAMLprim value ml_NSMenu_addItem(NSMenu *menu, value title_v) {
  CAMLparam1(title_v);

  NSString *title = [NSString stringWithUTF8String:String_val(title_v)];

  NSMenuItem *item =
      [menu addItemWithTitle:title action:NULL keyEquivalent:@""];

  CAMLreturn((value)item);
}

void ml_NSMenu_removeItem(NSMenu *menu, NSMenuItem *item) {
  [menu removeItem:item];
}

void ml_NSMenu_addSeparatorItem(NSMenu *menu) {
  [menu addItem:[NSMenuItem separatorItem]];
}

CAMLprim value ml_NSMenuItem_setTarget(NSMenuItem *item, value target_v) {
  CAMLparam1(target_v);

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

CAMLprim value ml_NSMenuItem_setAction(NSMenuItem *item, value action_v) {
  CAMLparam1(action_v);

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

void ml_NSMenu_setSubmenu(NSMenu *menu, NSMenu *submenu, NSMenuItem *item) {
  [menu setSubmenu:submenu forItem:item];
}
