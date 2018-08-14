#include <Cocoa/Cocoa.h>

#define CAML_NAME_SPACE

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/threads.h>

#define Val_NSApplication(v) ((value)(v))
#define NSApplication_val(v) ((__bridge NSApplication *)(value)(v))

#define Val_NSWindow(v) ((value)(v))
#define NSWindow_val(v) ((__bridge NSWindow *)(value)(v))

#define Val_NSView(v) ((value)(v))
#define NSView_val(v) ((__bridge NSView *)(value)(v))

#define Val_NSButton(v) ((value)(v))
#define NSButton_val(v) ((__bridge NSButton *)(value)(v))

static NSMutableDictionary *ml_NSViews;
static NSMutableArray *ml_NSViews_all;
dispatch_queue_t ml_q;

CAMLprim value ml_NSLog(value str)
{
  CAMLparam1(str);
  NSLog(@"%s", String_val(str));
  CAMLreturn(Val_unit);
}

@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>

- (void)makeMainMenu;

- (void)makeApplicationMenu:(NSMenu *)aMenu;
- (void)makeEditMenu:(NSMenu *)aMenu;
- (void)makeFileMenu:(NSMenu *)aMenu;
- (void)makeFindMenu:(NSMenu *)aMenu;
- (void)makeHelpMenu:(NSMenu *)aMenu;
- (void)makeSpellingMenu:(NSMenu *)aMenu;
- (void)makeWindowMenu:(NSMenu *)aMenu;

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
  ml_NSViews = [NSMutableDictionary new];
  ml_NSViews_all = [NSMutableArray new];
  return self;
}

- (void)applicationWillTerminate:(NSNotification *)__unused not
{
  // Insert code here to tear down your application
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not
{
  static value *closure_f = NULL;

  [self makeMainMenu];

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
  // ml_q = dispatch_queue_create(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

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

- (void)makeMainMenu
{
  NSMenu *mainMenu = [[NSMenu alloc] initWithTitle:@"MainMenu"];

  NSMenuItem *menuItem;
  NSMenu *submenu;

  // The titles of the menu items are for identification purposes only and shouldn't be localized.
  // The strings in the menu bar come from the submenu titles,
  // except for the application menu, whose title is ignored at runtime.
  menuItem = [mainMenu addItemWithTitle:@"Apple" action:NULL keyEquivalent:@""];
  submenu = [[NSMenu alloc] initWithTitle:@"Apple"];
  [NSApp performSelector:@selector(setAppleMenu:) withObject:submenu];
  [self makeApplicationMenu:submenu];
  [mainMenu setSubmenu:submenu forItem:menuItem];

  menuItem = [mainMenu addItemWithTitle:@"File" action:NULL keyEquivalent:@""];
  submenu = [[NSMenu alloc] initWithTitle:@"File"];
  [self makeFileMenu:submenu];
  [mainMenu setSubmenu:submenu forItem:menuItem];

  menuItem = [mainMenu addItemWithTitle:@"Edit" action:NULL keyEquivalent:@""];
  submenu = [[NSMenu alloc] initWithTitle:@"Edit"];
  [self makeEditMenu:submenu];
  [mainMenu setSubmenu:submenu forItem:menuItem];

  menuItem = [mainMenu addItemWithTitle:@"Window" action:NULL keyEquivalent:@""];
  submenu = [[NSMenu alloc] initWithTitle:@"Window"];
  [self makeWindowMenu:submenu];
  [mainMenu setSubmenu:submenu forItem:menuItem];
  [NSApp setWindowsMenu:submenu];

  menuItem = [mainMenu addItemWithTitle:@"Help" action:NULL keyEquivalent:@""];
  submenu = [[NSMenu alloc] initWithTitle:@"Help"];
  [self makeHelpMenu:submenu];
  [mainMenu setSubmenu:submenu forItem:menuItem];

  [NSApp setMainMenu:mainMenu];
}

- (void)makeApplicationMenu:(NSMenu *)aMenu
{
  NSString *applicationName = @"BriskMac";
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", @"About", applicationName]
                              action:@selector(orderFrontStandardAboutPanel:)
                       keyEquivalent:@""];
  [menuItem setTarget:NSApp];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Preferences..."
                              action:NULL
                       keyEquivalent:@","];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Services"
                              action:NULL
                       keyEquivalent:@""];
  NSMenu *servicesMenu = [[NSMenu alloc] initWithTitle:@"Services"];
  [aMenu setSubmenu:servicesMenu forItem:menuItem];
  [NSApp setServicesMenu:servicesMenu];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", @"Hide", applicationName]
                              action:@selector(hide:)
                       keyEquivalent:@"h"];
  [menuItem setTarget:NSApp];

  menuItem = [aMenu addItemWithTitle:@"Hide Others"
                              action:@selector(hideOtherApplications:)
                       keyEquivalent:@"h"];
  [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand | NSEventModifierFlagOption];
  [menuItem setTarget:NSApp];

  menuItem = [aMenu addItemWithTitle:@"Show All"
                              action:@selector(unhideAllApplications:)
                       keyEquivalent:@""];
  [menuItem setTarget:NSApp];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", @"Quit", applicationName]
                              action:@selector(terminate:)
                       keyEquivalent:@"q"];
  [menuItem setTarget:NSApp];
}

- (void)makeEditMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:@"Undo"
                              action:@selector(undo:)
                       keyEquivalent:@"z"];

  menuItem = [aMenu addItemWithTitle:@"Redo"
                              action:@selector(redo:)
                       keyEquivalent:@"Z"];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Cut"
                              action:@selector(cut:)
                       keyEquivalent:@"x"];

  menuItem = [aMenu addItemWithTitle:@"Copy"
                              action:@selector(copy:)
                       keyEquivalent:@"c"];

  menuItem = [aMenu addItemWithTitle:@"Paste"
                              action:@selector(paste:)
                       keyEquivalent:@"v"];

  menuItem = [aMenu addItemWithTitle:@"Paste and Match Style"
                              action:@selector(pasteAsPlainText:)
                       keyEquivalent:@"V"];
  [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand | NSEventModifierFlagOption];

  menuItem = [aMenu addItemWithTitle:@"Delete"
                              action:@selector(delete:)
                       keyEquivalent:@""];

  menuItem = [aMenu addItemWithTitle:@"Select All"
                              action:@selector(selectAll:)
                       keyEquivalent:@"a"];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Find"
                              action:NULL
                       keyEquivalent:@""];
  NSMenu *findMenu = [[NSMenu alloc] initWithTitle:@"Find"];
  [self makeFindMenu:findMenu];
  [aMenu setSubmenu:findMenu forItem:menuItem];

  menuItem = [aMenu addItemWithTitle:@"Spelling"
                              action:NULL
                       keyEquivalent:@""];
  NSMenu *spellingMenu = [[NSMenu alloc] initWithTitle:@"Spelling"];
  [self makeSpellingMenu:spellingMenu];
  [aMenu setSubmenu:spellingMenu forItem:menuItem];
}

- (void)makeFileMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:@"New"
                              action:NULL
                       keyEquivalent:@"n"];

  menuItem = [aMenu addItemWithTitle:@"Open..."
                              action:NULL
                       keyEquivalent:@"o"];

  menuItem = [aMenu addItemWithTitle:@"Open Recent"
                              action:NULL
                       keyEquivalent:@""];
  NSMenu *openRecentMenu = [[NSMenu alloc] initWithTitle:@"Open Recent"];
  [openRecentMenu performSelector:@selector(_setMenuName:) withObject:@"NSRecentDocumentsMenu"];
  [aMenu setSubmenu:openRecentMenu forItem:menuItem];

  menuItem = [openRecentMenu addItemWithTitle:@"Clear Menu"
                                       action:@selector(clearRecentDocuments:)
                                keyEquivalent:@""];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Close"
                              action:@selector(performClose:)
                       keyEquivalent:@"w"];

  menuItem = [aMenu addItemWithTitle:@"Save"
                              action:NULL
                       keyEquivalent:@"s"];

  menuItem = [aMenu addItemWithTitle:@"Save As..."
                              action:NULL
                       keyEquivalent:@"S"];

  menuItem = [aMenu addItemWithTitle:@"Revert"
                              action:NULL
                       keyEquivalent:@""];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Page Setup..."
                              action:@selector(runPageLayout:)
                       keyEquivalent:@"P"];

  menuItem = [aMenu addItemWithTitle:@"Print..."
                              action:@selector(print:)
                       keyEquivalent:@"p"];
}

- (void)makeFindMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:@"Find..."
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"f"];
  [menuItem setTag:NSFindPanelActionShowFindPanel];

  menuItem = [aMenu addItemWithTitle:@"Find Next"
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"g"];
  [menuItem setTag:NSFindPanelActionNext];

  menuItem = [aMenu addItemWithTitle:@"Find Previous"
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"G"];
  [menuItem setTag:NSFindPanelActionPrevious];

  menuItem = [aMenu addItemWithTitle:@"Use Selection for Find"
                              action:@selector(performFindPanelAction:)
                       keyEquivalent:@"e"];
  [menuItem setTag:NSFindPanelActionSetFindString];

  menuItem = [aMenu addItemWithTitle:@"Jump to Selection"
                              action:@selector(centerSelectionInVisibleArea:)
                       keyEquivalent:@"j"];
}

- (void)makeHelpMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", @"BriskMac", @"Help"]
                              action:@selector(showHelp:)
                       keyEquivalent:@"?"];
  [menuItem setTarget:NSApp];
}

- (void)makeSpellingMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:@"Spelling..."
                              action:@selector(showGuessPanel:)
                       keyEquivalent:@":"];

  menuItem = [aMenu addItemWithTitle:@"Check Spelling"
                              action:@selector(checkSpelling:)
                       keyEquivalent:@";"];

  menuItem = [aMenu addItemWithTitle:@"Check Spelling as You Type"
                              action:@selector(toggleContinuousSpellChecking:)
                       keyEquivalent:@""];
}

- (void)makeWindowMenu:(NSMenu *)aMenu
{
  NSMenuItem *menuItem;

  menuItem = [aMenu addItemWithTitle:@"Minimize"
                              action:@selector(performMinimize:)
                       keyEquivalent:@"m"];

  menuItem = [aMenu addItemWithTitle:@"Zoom"
                              action:@selector(performZoom:)
                       keyEquivalent:@""];

  [aMenu addItem:[NSMenuItem separatorItem]];

  menuItem = [aMenu addItemWithTitle:@"Bring All to Front"
                              action:@selector(arrangeInFront:)
                       keyEquivalent:@""];
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
  value windowId;
}

- (instancetype)initWithId:(value)winId
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

  caml_callback2(*closure_f, windowId, Val_int(WindowDidResize));
}

@end

CAMLprim value ml_NSWindow_makeWithContentRect(value winId, value x_v, value y_v, value w_v, value h_v)
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

CAMLprim value ml_NSWindow_center(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win center];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_makeKeyAndOrderFront(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  [win makeKeyAndOrderFront:nil];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_isVisible(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  BOOL b = [win isVisible];

  CAMLreturn(Val_bool(b));
}

CAMLprim value ml_NSWindow_contentView(value win_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);

  CAMLreturn(Val_NSView(win.contentView));
}

CAMLprim value ml_NSWindow_title(value win_v)
{
  CAMLparam1(win_v);
  CAMLlocal1(str_v);

  NSWindow *win = NSWindow_val(win_v);
  NSString *str = [win title];
  const char *bytes = [str UTF8String];
  int len = strlen(bytes);
  str_v = caml_alloc_string(len);
  memcpy(String_val(str_v), bytes, len);

  CAMLreturn(str_v);
}

CAMLprim value ml_NSWindow_setContentView(value win_v, value view_v)
{
  CAMLparam1(win_v);
  NSWindow *win = NSWindow_val(win_v);
  NSView *view = NSView_val(view_v);

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:10 green:10 blue:10 alpha:1] CGColor]];
  [win setContentView:view];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_setTitle(value win_v, value str_v)
{
  CAMLparam2(win_v, str_v);
  NSWindow *win = NSWindow_val(win_v);
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

// NSView
CAMLprim value ml_NSView_make()
{
  CAMLparam0();

  NSView *view = nil;
  view = [NSView new];

  [ml_NSViews_all addObject:view];

  CAMLreturn(Val_NSView(view));
}

CAMLprim value ml_NSView_memoize(value id_v, value view_v)
{
  CAMLparam2(id_v, view_v);
  NSView *view = NSView_val(view_v);

  [ml_NSViews setObject:view forKey:@(Int_val(id_v))];

  CAMLreturn(Val_unit);
}

CAMLprim value NSView_memoize(intnat id_, NSView *view)
{
  [ml_NSViews setObject:view forKey:@(id_)];

  return Val_unit;
}

CAMLprim value ml_NSView_free(value id_v)
{
  CAMLparam1(id_v);

  [ml_NSViews removeObjectForKey:@(Int_val(id_v))];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_addSubview(value view_v, value child_v)
{
  CAMLparam2(view_v, child_v);
  NSView *view = NSView_val(view_v);
  NSView *child = NSView_val(child_v);

  if ([NSThread isMainThread])
  {
    NSLog(@"MAIN");
    [view addSubview:child];
  }
  else
  {
    dispatch_sync(dispatch_get_main_queue(), ^{
      NSLog(@"NOT MAIN");
      [view addSubview:child];
    });
  }

  CAMLreturn(Val_NSView(view));
}

CAMLprim value NSView_addSubview(NSView *view, NSView *child)
{
  if ([NSThread isMainThread])
  {
    NSLog(@"MAIN");
    [view addSubview:child];
  }
  else
  {
    dispatch_sync(dispatch_get_main_queue(), ^{
      NSLog(@"NOT MAIN");
      [view addSubview:child];
    });
  }

  return Val_NSView(view);
}

// CAMLprim value ml_NSView_insertSubviewAt(NSView *view, NSView *child, intnat pos_)
// {
//   [view addSubview:child];
//   [view insertSubview:child atIndex:pos_];

//   CAMLreturn(Val_unit);
// }

CAMLprim value ml_NSView_setFrame(value view_v, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(view_v, x_v, y_v, w_v, h_v);

  NSView *view = NSView_val(view_v);

  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];

  CAMLreturn(Val_unit);
}

CAMLprim value NSView_setFrame(NSView *view, double x, double y, double w, double h)
{
  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];

  return Val_unit;
}

CAMLprim value ml_NSView_setBorderWidth(value view_v, value width_v)
{
  CAMLparam2(view_v, width_v);

  NSView *view = NSView_val(view_v);

  [view.layer setBorderWidth:Int_val(width_v)];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setBorderColor(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, blue_v, green_v, alpha_v);

  NSView *view = NSView_val(view_v);

  CGFloat red = Double_val(red_v);
  CGFloat blue = Double_val(blue_v);
  CGFloat green = Double_val(green_v);
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBorderColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSView_setBackgroundColor(value view_v, value red_v, value green_v, value blue_v, value alpha_v)
{
  CAMLparam5(view_v, red_v, blue_v, green_v, alpha_v);

  NSView *view = NSView_val(view_v);

  CGFloat red = Double_val(red_v);
  CGFloat blue = Double_val(blue_v);
  CGFloat green = Double_val(green_v);
  CGFloat alpha = Double_val(alpha_v);

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];

  CAMLreturn(Val_unit);
}

// NSButton
CAMLprim value ml_NSButton_make()
{
  CAMLparam0();

  NSButton *btn = nil;
  btn = [NSButton new];
  [ml_NSViews_all addObject:btn];

  CAMLreturn(Val_NSButton(btn));
}

CAMLprim value ml_NSButton_memoize(value id_v, value btn_v)
{
  CAMLparam2(id_v, btn_v);
  NSButton *btn = NSButton_val(btn_v);

  [ml_NSViews setObject:btn forKey:@(Int_val(id_v))];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_free(value id_v)
{
  CAMLparam1(id_v);

  [ml_NSViews removeObjectForKey:@(Int_val(id_v))];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setFrame(value btn_v, value x_v, value y_v, value w_v, value h_v)
{
  CAMLparam5(btn_v, x_v, y_v, w_v, h_v);

  NSButton *btn = NSButton_val(btn_v);

  CGFloat x = Double_val(x_v);
  CGFloat y = Double_val(y_v);
  CGFloat w = Double_val(w_v);
  CGFloat h = Double_val(h_v);

  NSRect rect = NSMakeRect(x, y, w, h);
  [btn setFrame:rect];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setCallback(value btn_v, value callback_v)
{
  CAMLparam2(btn_v, callback_v);

  NSButton *btn = NSButton_val(btn_v);
  // value __block callback = callback_v;

  // caml_register_global_root(&callback);

  // [btn setAction:^{
  //   caml_callback(callback, Val_unit);
  // }];

  CAMLreturn(Val_NSButton(btn));
}

CAMLprim value ml_NSButton_setTitle(value btn_v, value str_v)
{
  CAMLparam2(btn_v, str_v);
  NSButton *btn = NSButton_val(btn_v);
  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn setTitle:str];
  NSLog(@"setting title: %@", str);

  CAMLreturn(Val_NSButton(btn));
}
