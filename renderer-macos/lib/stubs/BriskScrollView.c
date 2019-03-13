#import "BriskViewable.h"

@interface BriskScrollView: NSScrollView
@end

@implementation BriskScrollView {
  value onScroll;
}

- (BOOL)isFlipped {
  return NO;
}

- (void)setOnScroll:(value)callbackOption {
  if (onScroll) {
    if (Is_block(callbackOption)) {
      caml_modify_generational_global_root(&onScroll, Field(callbackOption, 0));
    } else {
      caml_remove_generational_global_root(&onScroll);
      [[NSNotificationCenter defaultCenter] 
        removeObserver:self 
        name:NSViewBoundsDidChangeNotification
        object:self.contentView]; 
      self.contentView.postsBoundsChangedNotifications = NO;
      onScroll = (value)NULL;
    }
  } else if (Is_block(callbackOption)) {
    self.contentView.postsBoundsChangedNotifications = YES;
    [[NSNotificationCenter defaultCenter] 
      addObserver:self 
      selector:@selector(boundsDidChange) 
      name:NSViewBoundsDidChangeNotification 
      object:self.contentView]; 
    onScroll = Field(callbackOption, 0);
    caml_register_generational_global_root(&onScroll);
  }
}

- (void)boundsDidChange {
  if (onScroll) {
    NSRect visibleRect = [self.contentView documentVisibleRect];
    /* I know - this is confusing. self.contentSize is the size of the whole scrollable content (including the invisible area) */
    NSSize contentSize = self.documentView.frame.size;
    /* The size of the content chunk visible to the user */
    NSSize visibleAreaSize = self.contentSize;
    /* TODO: Fix this to happen synchronously. Otherwise we'll end up with
     * issues similar to RN with one frame late updates.
     */
    dispatch_async(dispatch_get_main_queue(), ^{
    value args[] = {
        caml_copy_double(visibleRect.origin.x),
        caml_copy_double(visibleRect.origin.y), 
        caml_copy_double(contentSize.width),
        caml_copy_double(contentSize.height),
        caml_copy_double(visibleAreaSize.width),
        caml_copy_double(visibleAreaSize.height)
      };
    brisk_caml_call_n(
      onScroll, 
      6, 
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
