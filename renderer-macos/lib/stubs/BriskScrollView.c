#import "BriskViewable.h"

@interface BriskScrollView: NSScrollView
@end

@interface BriskScrollView ()
@property(nonatomic, assign) value _callback;
@end

@implementation BriskScrollView

- (BOOL)isFlipped {
  return NO;
}

- (void)setOnScroll:(value)callbackOption {
  if (self._callback) {
    value prevCallback = self._callback;
    caml_remove_global_root(&prevCallback);
  }
  if (Is_block(callbackOption)) {
    self.contentView.postsBoundsChangedNotifications = YES;
    [[NSNotificationCenter defaultCenter] 
      addObserver:self 
      selector:@selector(boundsDidChange) 
      name:NSViewBoundsDidChangeNotification 
      object:self.contentView]; 
    value callback = Field(callbackOption, 0);
    caml_register_global_root(&callback);
    self._callback = callback;
  } else {
    self._callback = 0;
    [[NSNotificationCenter defaultCenter] 
      removeObserver:self 
      name:NSViewBoundsDidChangeNotification
      object:self.contentView]; 
    self.contentView.postsBoundsChangedNotifications = NO;
  }
}

- (void)boundsDidChange {
  if (self._callback) {
    NSRect visibleRect = [self.contentView documentVisibleRect];
    NSSize contentSize = self.contentSize;
    dispatch_async(dispatch_get_main_queue(), ^{
    value args[] = {
        caml_copy_double(visibleRect.origin.x),
        caml_copy_double(visibleRect.origin.y), 
        caml_copy_double(contentSize.width),
        caml_copy_double(contentSize.height)
      };
    brisk_caml_call_n(
      self._callback, 
      4, 
      args 
    );
  }
  );
}
}

@end

@interface BriskDocumentViewContainer : NSView
@end

@implementation BriskDocumentViewContainer
- (BOOL)isFlipped {
  return YES;
}
@end

BriskScrollView *ml_BriskScrollView_make() {
  BriskScrollView *scroll = [BriskScrollView new];
  retainView(scroll);

  [scroll setWantsLayer:YES];
  [scroll setBorderType:NSNoBorder];
  [scroll setDrawsBackground:NO];
  [scroll setHasVerticalScroller:YES];
  [scroll setHasHorizontalScroller:YES];
  scroll.documentView = [BriskDocumentViewContainer new];

  return scroll;
}

NSView *ml_BriskScrollView_documentView(BriskScrollView *scroll) {
  return scroll.documentView;
}

CAMLprim value ml_BriskScrollView_setOnScroll(BriskScrollView *scrollView, value callback_v) {
  CAMLparam1(callback_v);
  value callback = callback_v;

  [scrollView setOnScroll:callback];

  CAMLreturn(Val_unit);
}
