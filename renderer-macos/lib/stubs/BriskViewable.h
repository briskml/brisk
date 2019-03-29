#import "BriskCocoa.h"

/*
 * This protocol allows to wrap our views in additional containers.
 * That way we can manage the views and implement things like
 * responding to events etc.
 */
@protocol BriskView
- (NSView *)NSView;
@end

@protocol BriskViewParent

- (void)brisk_insertNode:(NSView *)child position:(intnat)position;

- (void)brisk_deleteNode:(NSView *)child;
@end

@protocol BriskMeasuredView
- (void)brisk_setFrame:(NSRect)rect
           paddingLeft:(CGFloat)left
          paddingRight:(CGFloat)right
            paddingTop:(CGFloat)top
         paddingBottom:(CGFloat)bottom;
@end

NSView *NSViewFromBriskView(id view);
