#import "Cocoa_stubs.h"

@interface BriskImage : NSView

@property(nonatomic, assign) NSImage *src;

- (void)setSource:(NSImage *)source;

@end

double ml_BriskImage_getImageWidth(BriskImage *img);
double ml_BriskImage_getImageHeight(BriskImage *img);
