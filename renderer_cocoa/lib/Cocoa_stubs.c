#include "brisk_cocoa.h"

static NSMutableDictionary *ml_Views;
static NSMutableArray *ml_Views_all;

CAMLprim value ml_NSLog(value str) {
  CAMLparam1(str);
  NSLog(@"%s", String_val(str));
  CAMLreturn(Val_unit);
}

value Val_some(value some_v) {
  CAMLparam1(some_v);
  CAMLlocal1(some);

  some = caml_alloc(1, 0);
  Store_field(some, 0, some_v);

  CAMLreturn(some);
}

@implementation View

@end

@implementation Button

- (void)onClick:(Callback)action {
  self._callback = action;
  [self setTarget:self];
  [self setAction:@selector(performCallback:)];
}

- (void)performCallback:(id)__unused sender {
  self._callback();
}
@end

@implementation MLApplicationDelegate {
  intnat applicationId;
}

- (instancetype)initWithId:(intnat)appId {
  if (self = [super init]) {
    applicationId = appId;
  }
  ml_Views = [NSMutableDictionary new];
  ml_Views_all = [NSMutableArray new];
  return self;
}

- (void)applicationWillTerminate:(NSNotification *)__unused not{
  // applicationWillTerminate
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;

  if (closure_f == NULL) {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL) {
    caml_callback2(*closure_f, Val_int(applicationId),
                   Val_int(ApplicationWillFinishLaunching));
  }
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;

  if (closure_f == NULL) {
    closure_f = caml_named_value("NSApp.delegate");
  }

  if (closure_f != NULL) {
    caml_callback2(*closure_f, Val_int(applicationId),
                   Val_int(ApplicationDidFinishLaunching));
  }
}

@end

@implementation MLWindowDelegate {
  intnat windowId;
}

- (instancetype)initWithId:(intnat)winId {
  if (self = [super init]) {
    windowId = winId;
  }
  return self;
}

- (void)windowDidResize:(NSNotification *)__unused aNotification {
  static value *closure_f = NULL;

  if (closure_f == NULL) {
    closure_f = caml_named_value("NSWindow.delegate");
  }

  caml_callback2(*closure_f, Val_int(windowId), Val_int(WindowDidResize));
}

@end

NSWindow *ml_NSWindow_makeWithContentRect(intnat winId, double x, double y,
                                          double w, double h) {
  NSRect contentRect = NSMakeRect(x, y, w, h);

  NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                         NSWindowStyleMaskMiniaturizable |
                         NSWindowStyleMaskResizable;

  NSWindow *win = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  [win setDelegate:[[MLWindowDelegate alloc] initWithId:winId]];

  return win;
}

CAMLprim value ml_NSWindow_makeWithContentRect_bc(value winId, value x_v,
                                                  value y_v, value w_v,
                                                  value h_v) {
  CAMLparam5(winId, x_v, y_v, w_v, h_v);

  NSWindow *win = ml_NSWindow_makeWithContentRect(
      Int_val(winId), Double_val(x_v), Double_val(y_v), Double_val(w_v),
      Double_val(h_v));

  CAMLreturn(Val_NSWindow(win));
}

void ml_NSWindow_center(NSWindow *win) { [win center]; }

void ml_NSWindow_makeKeyAndOrderFront(NSWindow *win) {
  [win makeKeyAndOrderFront:nil];
}

BOOL ml_NSWindow_isVisible(NSWindow *win) { return [win isVisible]; }

View *ml_NSWindow_contentView(NSWindow *win) { return win.contentView; }

void ml_NSWindow_setContentView(NSWindow *win, View *view) {
  [view setWantsLayer:YES];
  [win setContentView:view];
}

double ml_NSWindow_contentHeight(NSWindow *win) {
  return (double)[win contentRectForFrameRect:win.frame].size.height;
}

double ml_NSWindow_contentWidth(NSWindow *win) {
  return (double)[win contentRectForFrameRect:win.frame].size.width;
}

CAMLprim value ml_NSWindow_title(NSWindow *win) {
  CAMLparam0();
  CAMLlocal1(str_v);

  NSString *str = [win title];
  const char *bytes = [str UTF8String];
  int len = strlen(bytes);
  str_v = caml_alloc_string(len);
  memcpy(String_val(str_v), bytes, len);

  CAMLreturn(str_v);
}

CAMLprim value ml_NSWindow_setTitle(NSWindow *win, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [win setTitle:str];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSApplication_NSApp(value appId) {
  CAMLparam1(appId);
  NSApplication *app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];
  [app activateIgnoringOtherApps:YES];
  [app setDelegate:[[MLApplicationDelegate alloc] initWithId:Int_val(appId)]];

  CAMLreturn(Val_NSApplication(app));
}

void ml_NSApplication_run(NSApplication *app) {
  @autoreleasepool {
    [app run];
  }
}

void ml_registerLoop(value callback_v) {
  CAMLparam1(callback_v);

  caml_register_generational_global_root(&callback_v);

  CFRunLoopObserverRef observer = CFRunLoopObserverCreateWithHandler(
      NULL, kCFRunLoopAllActivities, YES, 0,
      ^(CFRunLoopObserverRef observerRef __unused,
        CFRunLoopActivity activity __unused) {
        caml_callback(callback_v, Val_unit);
      });

  CFRunLoopAddObserver(CFRunLoopGetCurrent(), observer, kCFRunLoopCommonModes);
  CAMLreturn0;
}

// View
View *ml_NSView_make() {
  View *view = [View new];
  [ml_Views_all addObject:view];

  return view;
}

void ml_NSView_memoize(intnat id_v, View *view) {
  [ml_Views setObject:view forKey:@(id_v)];
}

CAMLprim value ml_NSView_memoize_bc(value id_v, View *view) {
  CAMLparam1(id_v);

  ml_NSView_memoize(Int_val(id_v), view);

  CAMLreturn(Val_unit);
}

void ml_NSView_free(intnat id_v) { [ml_Views removeObjectForKey:@(id_v)]; }

CAMLprim value ml_NSView_free_bc(value id_v) {
  CAMLparam1(id_v);

  ml_NSView_free(Int_val(id_v));

  CAMLreturn(Val_unit);
}

void ml_NSView_addSubview(View *view, View *child) { [view addSubview:child]; }

void ml_NSView_removeSubview(View *child) { [child removeFromSuperview]; }

void ml_NSView_setFrame(View *view, double x, double y, double w, double h) {
  NSRect rect = NSMakeRect(x, y, w, h);
  [view setFrame:rect];
}

CAMLprim value ml_NSView_setFrame_bc(View *view, value x_v, value y_v,
                                     value w_v, value h_v) {
  CAMLparam4(x_v, y_v, w_v, h_v);

  ml_NSView_setFrame(view, Double_val(x_v), Double_val(y_v), Double_val(w_v),
                     Double_val(h_v));

  CAMLreturn(Val_unit);
}

void ml_NSView_setBorderWidth(View *view, double width) {
  [view.layer setBorderWidth:width];
}

CAMLprim value ml_NSView_setBorderWidth_bc(View *view, value width_v) {
  CAMLparam1(width_v);

  ml_NSView_setBorderWidth(view, Double_val(width_v));

  CAMLreturn(Val_unit);
}

void ml_NSView_setBorderColor(View *view, double red_v, double green_v,
                              double blue_v, double alpha) {
  CGFloat red = red_v / 255;
  CGFloat green = green_v / 255;
  CGFloat blue = blue_v / 255;

  [view setWantsLayer:YES];
  [view.layer setBorderColor:[[NSColor colorWithRed:red
                                              green:green
                                               blue:blue
                                              alpha:alpha] CGColor]];
}

CAMLprim value ml_NSView_setBorderColor_bc(View *view, value red_v,
                                           value green_v, value blue_v,
                                           value alpha_v) {
  CAMLparam4(red_v, green_v, blue_v, alpha_v);

  ml_NSView_setBorderColor(view, Double_val(red_v), Double_val(green_v),
                           Double_val(blue_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_NSView_setBackgroundColor(View *view, double red_v, double green_v,
                                  double blue_v, double alpha) {
  CGFloat red = red_v / 255;
  CGFloat green = green_v / 255;
  CGFloat blue = blue_v / 255;

  [view setWantsLayer:YES];
  [view.layer setBackgroundColor:[[NSColor colorWithRed:red
                                                  green:green
                                                   blue:blue
                                                  alpha:alpha] CGColor]];
}

CAMLprim value ml_NSView_setBackgroundColor_bc(View *view, value red_v,
                                               value green_v, value blue_v,
                                               value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_NSView_setBackgroundColor(view, Double_val(red_v), Double_val(blue_v),
                               Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

// NSButton
Button *ml_NSButton_make() {
  Button *btn = [Button new];
  [ml_Views_all addObject:btn];

  return btn;
}

CAMLprim value ml_NSButton_setCallback(Button *btn, value callback_v) {
  CAMLparam1(callback_v);

  value callback = callback_v;

  caml_register_global_root(&callback);

  [btn onClick:^{
    caml_callback(callback, Val_unit);
  }];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setTitle(Button *btn, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn setTitle:str];

  CAMLreturn(Val_unit);
}
