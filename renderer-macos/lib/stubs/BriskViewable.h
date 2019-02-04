#import "BriskCocoa.h"

@protocol BriskViewable

- (void)brisk_insertNode:(NSView *)child position:(intnat)position;

- (void)brisk_setFrame:(NSRect)rect;

@end
