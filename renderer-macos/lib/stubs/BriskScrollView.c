#import "BriskViewable.h"

@interface BriskScrollView : NSScrollView
@end

@implementation BriskScrollView

- (BOOL)isFlipped {
  return NO;
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
