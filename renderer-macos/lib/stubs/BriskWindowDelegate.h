#import "Cocoa_stubs.h"

@interface BriskWindowDelegate : NSObject <NSWindowDelegate>

@property(nonatomic, assign) value didResizeCallback;

- (void)setOnWindowDidResize:(value)callback;

@end
