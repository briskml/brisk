#import "BriskView.h"

@interface BriskImage : BriskView

@property(nonatomic, assign) NSImage *src;

- (void)setSource:(NSImage *)source;

@end

@implementation BriskImage

- (id)init {
  self = [super init];
  if (self) {
    self.src = [NSImage new];
  }
  return self;
}

- (void)setSource:(NSImage *)source {
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

CAMLprim value ml_BriskImage_setSourceFile(BriskImage *img, value source_v) {
  CAMLparam1(source_v);

  NSString *source = [NSString stringWithUTF8String:String_val(source_v)];
  NSImage *image = [[NSImage alloc] initWithContentsOfFile:source];

  if (image != NULL) {
    [img setSource:image];
  } else {
    NSLog(@"Image at path `%@` cannot be loaded", source);
  }

  CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskImage_setSourceNamed(BriskImage *img, value source_v) {
  CAMLparam1(source_v);

  NSString *source = [NSString stringWithUTF8String:String_val(source_v)];
  NSImage *image = [NSImage imageNamed:source];

  if (image != NULL) {
    [img setSource:image];
  } else {
    NSLog(@"Named image `%@` cannot be loaded", source);
  }

  CAMLreturn(Val_unit);
}
