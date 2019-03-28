#import "BriskCocoa.h"
#import "BriskImageHostingView.h"

@interface BriskImage : NSView <BriskImageHostingView>
@property(nonatomic, assign) NSImage *src;
@end

@implementation BriskImage

- (id)init {
    self = [super init];
    if (self) {
        self.src = [NSImage new];
    }
    return self;
}

- (void)brisk_setImage:(NSImage *)source {
    self.src = source;
    self.wantsLayer = true;
    [self.layer setContents:source];
}

@end

BriskImage *ml_BriskImage_make() {
    BriskImage *img = [BriskImage new];
    retainView(img);

    return img;
}

double ml_BriskImage_getImageWidth(BriskImage *img) {
    return (double)[img.src size].width;
}

double ml_BriskImage_getImageHeight(BriskImage *img) {
    return (double)[img.src size].height;
}
