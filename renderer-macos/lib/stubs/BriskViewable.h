#import "BriskCocoa.h"

@protocol BriskViewable

- (void)addChild:(NSView *)child;

- (void)setFrameRect:(NSRect)rect;

@end
