#import "BriskCocoa.h"

@protocol BriskImageHostingView
- (void)brisk_setImage:(NSImage *)image;
@end

CAMLprim value ml_BriskImageHostingView_setSourceFile(
    id<BriskImageHostingView> img, value source_v);
CAMLprim value ml_BriskImageHostingView_setSourceNamed(
    id<BriskImageHostingView> img, value source_v);
