#import "BriskViewable.h"

@interface BriskScrollView : NSScrollView <BriskViewable>

@end

@implementation BriskScrollView

- (id)init {
  self = [super init];

  if (self) {
    [self setBorderType:NSNoBorder];
    [self setHasVerticalScroller:YES];
    [self setHasHorizontalScroller:YES];
  }

  return self;
}

- (void)addChild:(NSView *)child {
  [self setDocumentView:child];
}

- (void)setFrameRect:(NSRect)rect {
  [self setFrame:rect];
}

@end

BriskScrollView *ml_BriskScrollView_make() {
  BriskScrollView *scroll = [BriskScrollView new];
  retainView(scroll);

  return scroll;
}
