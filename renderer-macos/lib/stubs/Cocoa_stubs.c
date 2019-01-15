#import "Cocoa_stubs.h"
#import "BriskApplicationDelegate.h"
#import "BriskWindowDelegate.h"
#import <caml/threads.h>

dispatch_semaphore_t caml_thread_sema;

value Val_some(value some_v) {
  CAMLparam1(some_v);
  CAMLlocal1(some);

  some = caml_alloc(1, 0);
  Store_field(some, 0, some_v);

  CAMLreturn(some);
}

CAMLprim value ml_NSLog(value str) {
  CAMLparam1(str);
  CAMLreturn(Val_unit);
}

NSWindow *ml_NSWindow_makeWithContentRect(double x, double y, double w,
                                          double h) {
  NSRect contentRect = NSMakeRect(x, y, w, h);

  NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                         NSWindowStyleMaskMiniaturizable |
                         NSWindowStyleMaskResizable;

  NSWindow *win = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
  [win setDelegate:[BriskWindowDelegate new]];

  return win;
}

CAMLprim value ml_NSWindow_setOnWindowDidResize(NSWindow *window,
                                                value callback) {
  CAMLparam1(callback);
  [(BriskWindowDelegate *)[window delegate] setOnWindowDidResize:callback];
  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSWindow_makeWithContentRect_bc(value x_v, value y_v,
                                                  value w_v, value h_v) {
  CAMLparam4(x_v, y_v, w_v, h_v);

  NSWindow *win = ml_NSWindow_makeWithContentRect(
      Double_val(x_v), Double_val(y_v), Double_val(w_v), Double_val(h_v));

  CAMLreturn((value)win);
}

void ml_NSWindow_center(NSWindow *win) { [win center]; }

void ml_NSWindow_makeKeyAndOrderFront(NSWindow *win) {
  [win makeKeyAndOrderFront:nil];
}

BOOL ml_NSWindow_isVisible(NSWindow *win) { return [win isVisible]; }

NSView *ml_NSWindow_contentView(NSWindow *win) { return win.contentView; }

void ml_NSWindow_setContentView(NSWindow *win, NSView *view) {
  [view setWantsLayer:YES];
  [win setContentView:view];
}

// A separate bytecode version is probably needed
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

NSMutableSet *retainedViews;

void brisk_init() {
  retainedViews = [NSMutableSet new];
  caml_thread_sema = dispatch_semaphore_create(1);
}

void ml_NSApplication_configure() {
  brisk_init();
  NSApplication *app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];
  [app activateIgnoringOtherApps:YES];
  [app setDelegate:[BriskApplicationDelegate new]];
}

void ml_NSApplication_main() {
  caml_release_runtime_system();
  @autoreleasepool {
    [[NSApplication sharedApplication] run];
  }
}

void ml_isMain() { NSLog(@"%i", [NSThread isMainThread]); }

CAMLprim void run_lwt_iter_if_needed() {
  /*
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
  ^{ brisk_caml_call(^{ intnat should_schedule =
  caml_callback(*caml_named_value("Brisk_lwt_iter"), Val_unit); if
  (should_schedule == 1) { ml_lwt_iter();
      }
    });
    ml_lwt_iter();
  });
  */
}

void brisk_caml_call(void (^block)()) {
  dispatch_semaphore_wait(caml_thread_sema, DISPATCH_TIME_FOREVER);
  // This should only be called when we call from outside of OCaml I suppose
  caml_c_thread_register();
  caml_acquire_runtime_system();
  block();
  caml_release_runtime_system();
  dispatch_semaphore_signal(caml_thread_sema);
  run_lwt_iter_if_needed();
}

CAMLprim void ml_lwt_iter() {
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                 ^{
                   brisk_caml_call(^{
                     /*intnat should_schedule = */ caml_callback(
                         *caml_named_value("Brisk_lwt_iter"), Val_unit);
                     // if (should_schedule == 1) {
                     // }
                   });
                   sleep(2);
                   ml_lwt_iter();
                 });
}

void flush_and_layout_sync() {
  caml_callback(*caml_named_value("Brisk_flush_layout"), Val_unit);
}

void ml_schedule_layout_flush() {
  dispatch_async(dispatch_get_main_queue(), ^{
    brisk_caml_call(^{
      flush_and_layout_sync();
    });
  });
}

void brisk_caml_call_and_flush(void (^block)()) {
  brisk_caml_call(^{
    block();
    flush_and_layout_sync();
  });
}

void retainView(NSView *view) { [retainedViews addObject:view]; }

void releaseView(NSView *view) { [retainedViews removeObject:view]; }
