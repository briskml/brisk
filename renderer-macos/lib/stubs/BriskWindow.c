#import "BriskCocoa.h"
#import "BriskWindowDelegate.h"

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

void ml_NSWindow_setOnWindowDidResize(NSWindow *window, value callback) {
  [(BriskWindowDelegate *)[window delegate] setOnWindowDidResize:callback];
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

double ml_NSWindow_contentHeight(NSWindow *win) {
  return (double)[win contentRectForFrameRect:win.frame].size.height;
}

value ml_NSWindow_contentHeight_bc(NSWindow *win) {
  CAMLparam0();
  CAMLlocal1(height);
  height =
      caml_copy_double([win contentRectForFrameRect:win.frame].size.height);
  CAMLreturn(height);
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

void ml_NSWindow_setContentIsFullSize(NSWindow *win, intnat isFullSize) {
  if (isFullSize == 1) {
    win.styleMask |= NSWindowStyleMaskFullSizeContentView;
  } else {
    win.styleMask &= ~NSWindowStyleMaskFullSizeContentView;
  }
}

CAMLprim value ml_NSWindow_setContentIsFullSize_bc(NSWindow *win,
                                                   value isFullSize_v) {
  CAMLparam1(isFullSize_v);
  ml_NSWindow_setContentIsFullSize(win, Int_val(isFullSize_v));

  CAMLreturn(Val_unit);
}

void ml_NSWindow_setTitleIsHidden(NSWindow *win, intnat hidden) {
  win.titleVisibility = hidden;
}

CAMLprim value ml_NSWindow_setTitleIsHidden_bc(NSWindow *win, value hidden_v) {
  CAMLparam1(hidden_v);
  ml_NSWindow_setTitleIsHidden(win, Int_val(hidden_v));

  CAMLreturn(Val_unit);
}

void ml_NSWindow_setTitlebarIsTransparent(NSWindow *win, intnat transparent) {
  win.titlebarAppearsTransparent = (BOOL)transparent;
}

CAMLprim value ml_NSWindow_setTitlebarIsTransparent_bc(NSWindow *win,
                                                       value transparent_v) {
  CAMLparam1(transparent_v);
  ml_NSWindow_setTitlebarIsTransparent(win, Int_val(transparent_v));

  CAMLreturn(Val_unit);
}
