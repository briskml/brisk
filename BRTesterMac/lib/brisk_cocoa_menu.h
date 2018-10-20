#ifndef BRISK_COCOA_MENU_H
#define BRISK_COCOA_MENU_H

#include "brisk_cocoa.h"

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

#endif /* BRISK_COCOA_MENU_H */
