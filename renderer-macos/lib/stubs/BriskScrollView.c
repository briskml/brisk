#import "BriskViewable.h"

@interface NSScrollView (BriskViewable) <BriskViewable>
@end

@implementation NSScrollView (BriskViewable)

- (void)brisk_insertNode:(NSView *)child position:(intnat)position {
  [self.documentView
      addSubview:child
      positioned:NSWindowAbove
      relativeTo:(position == 0 ? nil : self.subviews[position - 1])];
}

- (void)brisk_setFrame:(NSRect)rect {
  [self setFrame:rect];
}

@end

NSScrollView *ml_BriskScrollView_make() {
  NSScrollView *scroll = [NSScrollView new];
  retainView(scroll);

  [scroll setBorderType:NSNoBorder];
  [scroll setHasVerticalScroller:YES];
  [scroll setHasHorizontalScroller:YES];
  scroll.documentView = [NSView new];

  return scroll;
}

NSView *ml_BriskScrollView_documentView(NSScrollView *scroll) {
  return scroll.documentView;
}
