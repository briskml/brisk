#import "BriskViewable.h"

NSView *NSViewFromBriskView(id view) {
  if ([view conformsToProtocol:@protocol(BriskView)]) {
    return [view NSView];
  } else if ([view isKindOfClass:[NSView class]]) {
    return view;
  }
  return nil;
  NSCAssert(NO, @"Fatal error; this object cannot be converted to NSView");
}
