#import <Cocoa/Cocoa.h>
#define CAML_NAME_SPACE
#import "Cocoa_stubs.h"
#import <caml/mlvalues.h>

@interface BriskWindowDelegate : NSObject <NSWindowDelegate>

@property(nonatomic, assign) value didResizeCallback;

- (void)setOnWindowDidResize:(value)callback;

@end
