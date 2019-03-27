#import "BriskCocoa.h"

@protocol BriskViewable

- (void)brisk_insertNode:(NSView *)child position:(intnat)position;

- (void)brisk_deleteNode:(NSView *)child;

- (void)brisk_setFrame:(NSRect)rect
           paddingLeft:(CGFloat)left
          paddingRight:(CGFloat)right
            paddingTop:(CGFloat)top
         paddingBottom:(CGFloat)bottom;
@end
