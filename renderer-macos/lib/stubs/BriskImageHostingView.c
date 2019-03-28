#import "BriskImageHostingView.h"
CAMLprim value ml_BriskImageHostingView_setSourceFile(
    id<BriskImageHostingView> img, value source_v) {
    CAMLparam1(source_v);

    NSString *source = [NSString stringWithUTF8String:String_val(source_v)];
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:source];

    if (image != NULL) {
        [img brisk_setImage:image];
    } else {
        NSLog(@"Image at path `%@` cannot be loaded", source);
    }

    CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskImageHostingView_setSourceNamed(
    id<BriskImageHostingView> img, value source_v) {
    CAMLparam1(source_v);

    NSString *source = [NSString stringWithUTF8String:String_val(source_v)];
    NSImage *image = [NSImage imageNamed:source];

    if (image != NULL) {
        [img brisk_setImage:image];
    } else {
        NSLog(@"Named image `%@` cannot be loaded", source);
    }

    CAMLreturn(Val_unit);
}
