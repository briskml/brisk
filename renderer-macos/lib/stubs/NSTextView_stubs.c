#import "NSTextView_stubs.h"
#define CAML_NAME_SPACE
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

@implementation TextView

- (BOOL)isEditable {
  return NO;
}

- (BOOL)isSelectable {
  return NO;
}

@end

TextView *ml_NSTextView_make() {
  TextView *txt = [TextView new];
  retainView(txt);

  return txt;
}

CAMLprim value ml_NSTextView_setStringValue(TextView *txt, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  txt.string = str;

  CAMLreturn(Val_unit);
}

void ml_NSTextView_setBackgroundColor(TextView *txt, double red_v,
                                      double green_v, double blue_v,
                                      double alpha) {
  CGFloat red = red_v;
  CGFloat green = green_v;
  CGFloat blue = blue_v;

  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];

  [txt setBackgroundColor:color];
}

CAMLprim value ml_NSTextView_setBackgroundColor_bc(TextView *txt, value red_v,
                                                   value green_v, value blue_v,
                                                   value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_NSTextView_setBackgroundColor(txt, Double_val(red_v), Double_val(blue_v),
                                   Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}
