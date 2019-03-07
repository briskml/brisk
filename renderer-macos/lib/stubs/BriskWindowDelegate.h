#import "BriskCocoa.h"

@interface BriskWindowDelegate : NSObject <NSWindowDelegate>

- (void)setOnWindowDidResize:(value)callback;

@end
