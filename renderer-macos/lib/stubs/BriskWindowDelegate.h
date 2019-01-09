#import <Cocoa/Cocoa.h>
#define CAML_NAME_SPACE
#import <caml/mlvalues.h>
#import "Cocoa_stubs.h"

@interface BriskWindowDelegate : NSObject <NSWindowDelegate>

@property (nonatomic, assign) value didResizeCallback;

- (void)setOnWindowDidResize:(value)callback;

@end
