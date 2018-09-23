#import <Cocoa/Cocoa.h>

#define CAML_NAME_SPACE

#import <caml/mlvalues.h>
#import <caml/memory.h>
#import <caml/callback.h>
#import <caml/alloc.h>
#import <caml/bigarray.h>
#import <caml/custom.h>
#import <caml/fail.h>
#import <caml/threads.h>

#define Val_NSApplication(v) ((value)(v))
#define NSApplication_val(v) ((__bridge NSApplication *)(value)(v))

#define Val_NSWindow(v) ((value)(v))
#define NSWindow_val(v) ((__bridge NSWindow *)(value)(v))

#define Val_NSMenu(v) ((value)(v))
#define NSMenu_val(v) ((__bridge NSMenu *)(value)(v))

#define Val_NSMenuItem(v) ((value)(v))
#define NSMenuItem_val(v) ((__bridge NSMenuItem *)(value)(v))

#define Val_none Val_int(0)

// static value Val_some(value some_v) {
//   CAMLparam1(some_v);
//   CAMLlocal1(some);

//   some = caml_alloc(1, 0);
//   Store_field(some, 0, some_v);

//   CAMLreturn(some);
// }

#define Some_val(v) Field(v, 0)

@interface View : NSView
@end

@implementation View

- (BOOL)isFlipped
{
  return YES;
}

@end

typedef void (^ActionBlock)();

@interface Button : NSButton

@property(nonatomic, copy) ActionBlock _actionBlock;

- (void)onClick:(ActionBlock)action;
@end

@implementation Button

- (void)onClick:(ActionBlock)action
{
  self._actionBlock = action;
  [self setTarget:self];
  [self setAction:@selector(callActionBlock:)];
}

- (void)callActionBlock:(id)__unused sender
{
  self._actionBlock();
}
@end

#define Val_View(v) ((value)(v))
#define View_val(v) ((__bridge View *)(value)(v))

#define Val_Button(v) ((value)(v))
#define Button_val(v) ((__bridge Button *)(value)(v))

static NSMutableDictionary *ml_Views;
static NSMutableArray *ml_Views_all;

CAMLprim value ml_NSLog(value str)
{
  CAMLparam1(str);
  NSLog(@"%s", String_val(str));
  CAMLreturn(Val_unit);
}

@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>

@end

enum
{
  ApplicationWillFinishLaunching,
  ApplicationDidFinishLaunching
};

@implementation MLApplicationDelegate
{
  value applicationId;
}

- (instancetype)initWithId:(value)appId
{
  if (self = [super init])
  {
    applicationId = appId;
  }
  ml_Views = [NSMutableDictionary new];
  ml_Views_all = [NSMutableArray new];
  return self;
}

- (void)applicationWillTerminate:(NSNotification *)__unused not
{
  // applicationWillTerminate
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL)
  {
    caml_callback2(*closure_f, applicationId, Val_int(ApplicationWillFinishLaunching));
  }
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL)
  {
    caml_callback2(*closure_f, applicationId, Val_int(ApplicationDidFinishLaunching));
  }
}

@end

@interface MLWindowDelegate : NSObject <NSWindowDelegate>

@end

enum
{
  WindowDidResize
};

@implementation MLWindowDelegate
{
  intnat windowId;
}

- (instancetype)initWithId:(intnat)winId
{
  if (self = [super init])
  {
    windowId = winId;
  }
  return self;
}

- (void)windowDidResize:(NSNotification *)__unused aNotification
{
  static value *closure_f = NULL;

  if (closure_f == NULL)
  {
    closure_f = caml_named_value("NSWindow.delegate");
  }

  caml_callback2(*closure_f, Val_int(windowId), Val_int(WindowDidResize));
}

@end

CAMLprim value ml_NSWindow_makeWithContentRect_bc(value winId, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(winId, x_v, y_v, w_v, h_v);
  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect contentRect = NSMakeRect(x, y, w, h);

  NSUInteger styleMask =
      NSWindowStyleMaskTitled |
      NSWindowStyleMaskClosable |
      NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskResizable;

  NSWindow *win = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  [win setDelegate:[[MLWindowDelegate alloc] initWithId:winId]];

  CAMLreturn(Val_NSWindow(win));
}

NSWindow *ml_NSWindow_makeWithContentRect(intnat winId, double x, double y, double w, double h)
{
  NSRect contentRect = NSMakeRect(x, y, w, h);

  NSUInteger styleMask =
      NSWindowStyleMaskTitled |
      NSWindowStyleMaskClosable |
      NSWindowStyleMaskMiniaturizable |
      NSWindowStyleMaskResizable;

  NSWindow *win = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  [win setDelegate:[[MLWindowDelegate alloc] initWithId:winId]];

  return win;
}

CAMLprim value ml_NSWindow_center_bc(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win center];
  CAMLreturn(Val_unit);
}

void ml_NSWindow_center(NSWindow *win)
{
  [win center];
}

CAMLprim value ml_NSWindow_makeKeyAndOrderFront_bc(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win makeKeyAndOrderFront:nil];

  CAMLreturn(Val_unit);
}

void ml_NSWindow_makeKeyAndOrderFront(NSWindow *win)
{
  [win makeKeyAndOrderFront:nil];
}

CAMLprim value ml_NSWindow_isVisible_bc(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  BOOL b = [win isVisible];

  CAMLreturn(Val_bool(b));
}

BOOL ml_NSWindow_isVisible(NSWindow *win)
{
  BOOL b = [win isVisible];
  return b;
}

CAMLprim value ml_NSWindow_contentView_bc(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);

  CAMLreturn(Val_View(win.contentView));
}

View *ml_NSWindow_contentView(NSWindow *win)
{
  return win.contentView;
}

CAMLprim value ml_NSWindow_setContentView_bc(value win_v, value view_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  View *view = View_val(view_v);

  [view setWantsLayer:YES];

  [win setContentView:view];

  CAMLreturn(Val_unit);
}

void ml_NSWindow_setContentView(NSWindow *win, View *view)
{
  [view setWantsLayer:YES];
  [win setContentView:view];
}

CAMLprim value ml_NSWindow_title(NSWindow *win)
{
  CAMLparam0();
  CAMLlocal1(str_v);

  NSString *str = [win title];
  const char *bytes = [str UTF8String];
  int len = strlen(bytes);
  str_v = caml_alloc_string(len);
  memcpy(String_val(str_v), bytes, len);

  CAMLreturn(str_v);
}

CAMLprim value ml_NSWindow_setTitle(NSWindow *win, value str_v)
{
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [win setTitle:str];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSApplication_NSApp(value appId)
{
  CAMLparam1(appId);
  NSApplication *app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];
  [app activateIgnoringOtherApps:YES];
  [app setDelegate:[[MLApplicationDelegate alloc] initWithId:appId]];

  CAMLreturn(Val_NSApplication(app));
}

CAMLprim value ml_NSApplication_run(value appId)
{
  CAMLparam1(appId);

  @autoreleasepool
  {
    NSApplication *app = [NSApplication sharedApplication];
    [app run];
  }

  CAMLreturn(Val_unit);
}

enum {
  // Apple
  MActionOrderFrontStandardAboutPanel,
  MActionHide,
  MActionHideOtherApplications,
  MActionUnhideAllApplications,
  MActionTerminate,
  // File
  MActionClose,
  MActionRunPageLayout,
  MActionPrint,
  // Edit
  MActionUndo,
  MActionRedo,
  MActionCut,
  MActionCopy,
  MActionPaste,
  MActionPasteAsPlainText,
  MActionDelete,
  MActionSelectAll,
  // Find
  MActionFind,
  MActionFindNext,
  MActionFindPrevious,
  MActionFindUseSelection,
  MActionJumpToSelection,
  // Spelling
  MActionShowGuessPanel,
  MActionCheckSpelling,
  MActionCheckSpellingContinuously,
  // Window
  MActionMinimize,
  MActionZoom,
  MActionBringAllInFront,
  // Help
  MActionHelp
};

enum {
  MenuTargetNSApp,
};

enum {
  MenuKindApple,
  MenuKindMain,
  MenuKindServices,
  MenuKindWindows,
};

CAMLprim value ml_NSApplication_setMenu(value kind_v, value menu_v) {
  CAMLparam2(kind_v, menu_v);

  NSMenu *menu = NSMenu_val(menu_v);

  switch (Int_val(kind_v)) {
  case MenuKindApple:
    [NSApp performSelector:@selector(setAppleMenu:) withObject:menu];
    break;
  case MenuKindMain:
    [NSApp setMainMenu:menu];
    break;
  case MenuKindServices:
    [NSApp setServicesMenu:menu];
    break;
  case MenuKindWindows:
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

CAMLprim value ml_NSMenu_removeItem(value menu_v, value item_v) {
  CAMLparam2(menu_v, item_v);

  NSMenu *menu = NSMenu_val(menu_v);
  NSMenuItem *item = NSMenuItem_val(item_v);

  [menu removeItem:item];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenu_addSeparatorItem(value menu_v) {
  CAMLparam1(menu_v);

  NSMenu *menu = NSMenu_val(menu_v);
  [menu addItem:[NSMenuItem separatorItem]];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSMenuItem_setTarget(value item_v, value target_v) {
  CAMLparam2(item_v, target_v);

  NSMenuItem *item = NSMenuItem_val(item_v);

  if (Is_long(target_v)) {
    switch (Int_val(target_v)) {
    case MenuTargetNSApp:
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
  case MActionOrderFrontStandardAboutPanel:
    action = @selector(orderFrontStandardAboutPanel:);
    break;
  case MActionHide:
    action = @selector(hide:);
    break;
  case MActionHideOtherApplications:
    action = @selector(hideOtherApplications:);
    break;
  case MActionUnhideAllApplications:
    action = @selector(unhideAllApplications:);
    break;
  case MActionTerminate:
    action = @selector(terminate:);
    break;
  case MActionClose:
    action = @selector(performClose:);
    break;
  case MActionRunPageLayout:
    action = @selector(runPageLayout:);
    break;
  case MActionPrint:
    action = @selector(print:);
    break;
  case MActionUndo:
    action = @selector(undo:);
    break;
  case MActionRedo:
    action = @selector(redo:);
    break;
  case MActionCut:
    action = @selector(cut:);
    break;
  case MActionCopy:
    action = @selector(copy:);
    break;
  case MActionPaste:
    action = @selector(paste:);
    break;
  case MActionPasteAsPlainText:
    action = @selector(pasteAsPlainText:);
    break;
  case MActionDelete:
    action = @selector(delete:);
    break;
  case MActionSelectAll:
    action = @selector(selectAll:);
    break;
  case MActionFind:
    [item setTag:NSFindPanelActionShowFindPanel];
    action = @selector(performFindPanelAction:);
    break;
  case MActionFindNext:
    [item setTag:NSFindPanelActionNext];
    action = @selector(performFindPanelAction:);
    break;
  case MActionFindPrevious:
    [item setTag:NSFindPanelActionPrevious];
    action = @selector(performFindPanelAction:);
    break;
  case MActionFindUseSelection:
    [item setTag:NSFindPanelActionSetFindString];
    action = @selector(performFindPanelAction:);
    break;
  case MActionJumpToSelection:
    action = @selector(centerSelectionInVisibleArea:);
    break;
  case MActionShowGuessPanel:
    action = @selector(showGuessPanel:);
    break;
  case MActionCheckSpelling:
    action = @selector(checkSpelling:);
    break;
  case MActionCheckSpellingContinuously:
    action = @selector(toggleContinuousSpellChecking:);
    break;

  case MActionMinimize:
    action = @selector(performMinimize:);
    break;
  case MActionZoom:
    action = @selector(performZoom:);
    break;
  case MActionBringAllInFront:
    action = @selector(arrangeInFront:);
    break;
  case MActionHelp:
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

CAMLprim value ml_NSMenu_setSubmenu(value menu_v, value submenu_v,
                                    value item_v) {
  CAMLparam3(menu_v, submenu_v, item_v);

  NSMenu *menu = NSMenu_val(menu_v);
  NSMenu *submenu = NSMenu_val(submenu_v);
  NSMenuItem *item = NSMenuItem_val(item_v);

  [menu setSubmenu:submenu forItem:item];

  CAMLreturn(Val_unit);
}

// View
CAMLprim value ml_NSView_make_bc()
{
  CAMLparam0();

  View *view = [View new];

  [ml_Views_all addObject:view];

  CAMLreturn(Val_View(view));
}

View *ml_NSView_make()
{
  View *view = [View new];
  [ml_Views_all addObject:view];

  return view;
}

CAMLprim value ml_NSView_memoize_bc(value id_v, value view_v)
{
  CAMLparam2(id_v, view_v);
  View *view = View_val(view_v);

  [ml_Views setObject:view forKey:@(Int_val(id_v))];

  CAMLreturn(Val_unit);
}

void ml_NSView_memoize(intnat id_v, View *view)
{
  [ml_Views setObject:view forKey:@(id_v)];
}

CAMLprim value ml_NSView_free_bc(value id_v)
{
  CAMLparam1(id_v);

  [ml_Views removeObjectForKey:@(Int_val(id_v))];
  CAMLreturn(Val_unit);
}

void ml_NSView_free(intnat id_v)
{
  [ml_Views removeObjectForKey:@(id_v)];
}

CAMLprim value ml_NSView_addSubview_bc(value view_v, value child_v)
{
  CAMLparam2(view_v, child_v);
  View *view = View_val(view_v);
  View *child = View_val(child_v);

  [view addSubview:child];

  CAMLreturn(Val_unit);
}

void ml_NSView_addSubview(View *view, View *child)
{
  [view addSubview:child];
}

CAMLprim value ml_NSView_removeSubview_bc(value child_v)
{
  CAMLparam1(child_v);
  View *child = View_val(child_v);

  [child removeFromSuperview];

  CAMLreturn(Val_unit);
}

void ml_NSView_removeSubview(View *child)
{
  [child removeFromSuperview];
}

CAMLprim value ml_NSView_setFrame_bc(value view_v, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(view_v, x_v, y_v, w_v, h_v);

  View *view = View_val(view_v);

  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];

  CAMLreturn(Val_unit);
}

void ml_NSView_setFrame(View *view, double x, double y, double w, double h)
{
  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];
}

CAMLprim value ml_NSView_setBorderWidth_bc(value view_v, value width_v)
{
  CAMLparam2(view_v, width_v);

  View *view = View_val(view_v);

  [view.layer setBorderWidth:Int_val(width_v)];

  CAMLreturn(Val_unit);
}

void ml_NSView_setBorderWidth(View *view, double width)
{
  [view.layer setBorderWidth:width];
}

CAMLprim value ml_NSView_setBorderColor_bc(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, green_v, blue_v, alpha_v);

  View *view = View_val(view_v);

  CGFloat red = Double_val(red_v) / 255;
  CGFloat green = Double_val(green_v) / 255;
  CGFloat blue = Double_val(blue_v) / 255;
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBorderColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

void ml_NSView_setBorderColor(View *view, double red_v, double green_v, double blue_v, double alpha)
{
  CGFloat red = red_v / 255;
  CGFloat green = green_v / 255;
  CGFloat blue = blue_v / 255;

  [view setWantsLayer:YES];
  [view.layer setBorderColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];
}

CAMLprim value ml_NSView_setBackgroundColor_bc(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, blue_v, green_v, alpha_v);

  View *view = View_val(view_v);

  CGFloat red = Double_val(red_v) / 255;
  CGFloat blue = Double_val(blue_v) / 255;
  CGFloat green = Double_val(green_v) / 255;
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

void ml_NSView_setBackgroundColor(View *view, double red_v, double green_v, double blue_v, double alpha)
{
  CGFloat red = red_v / 255;
  CGFloat green = green_v / 255;
  CGFloat blue = blue_v / 255;

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];
}

// NSButton
CAMLprim value ml_NSButton_make_bc()
{
  CAMLparam0();

  Button *btn = [Button new];
  [ml_Views_all addObject:btn];

  CAMLreturn(Val_Button(btn));
}

Button *ml_NSButton_make()
{
  Button *btn = [Button new];
  [ml_Views_all addObject:btn];

  return btn;
}

CAMLprim value ml_NSButton_setCallback(Button *btn, value callback_v)
{
  CAMLparam1(callback_v);

  value callback = callback_v;

  caml_register_global_root(&callback);

  [btn onClick:^{
    caml_callback(callback, Val_unit);
  }];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setTitle(Button *btn, value str_v)
{
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn setTitle:str];

  CAMLreturn(Val_unit);
}
