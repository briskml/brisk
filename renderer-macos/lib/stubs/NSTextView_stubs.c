#import "NSTextView_stubs.h"
#define CAML_NAME_SPACE
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

@implementation TextView

- (id)init {
  self = [super init];
  if (self) {
    self.attributedString = [[NSMutableAttributedString alloc] init];
    self.attributedProps = [NSMutableDictionary dictionary];
  }
  return self;
}

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

void ml_NSTextView_applyAttributes(TextView *txt) {
  NSRange range = NSMakeRange(0, txt.attributedString.length);

  [txt.attributedString setAttributes:txt.attributedProps range:range];
  [[txt textStorage] setAttributedString:txt.attributedString];
}

double ml_NSTextView_getTextWidth(TextView *txt) {
  return (double)[txt.attributedString size].width;
}

double ml_NSTextView_getTextHeight(TextView *txt) {
  return (double)[txt.attributedString size].height;
}

CAMLprim value ml_NSTextView_getTextWidth_bc(TextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_NSTextView_getTextWidth(txt)));
}

CAMLprim value ml_NSTextView_getTextHeight_bc(TextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_NSTextView_getTextHeight(txt)));
}

CAMLprim value ml_NSTextView_setStringValue(TextView *txt, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];

  [txt.attributedString.mutableString setString:str];

  ml_NSTextView_applyAttributes(txt);

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSTextView_setFont(TextView *txt, value fontName_v,
                                     double fontSize) {
  CAMLparam1(fontName_v);

  NSString *fontName = [NSString stringWithUTF8String:String_val(fontName_v)];

  txt.attributedProps[NSFontAttributeName] =
      [NSFont fontWithName:fontName size:(CGFloat)fontSize];

  ml_NSTextView_applyAttributes(txt);

  CAMLreturn(Val_unit);
}

void ml_NSTextView_setColor(TextView *txt, double red, double green,
                            double blue, double alpha) {
  txt.attributedProps[NSForegroundColorAttributeName] =
      ml_NSColor_make(red, green, blue, alpha);

  ml_NSTextView_applyAttributes(txt);
}

CAMLprim value ml_NSTextView_setColor_bc(TextView *txt, value red_v,
                                         value green_v, value blue_v,
                                         value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_NSTextView_setColor(txt, Double_val(red_v), Double_val(blue_v),
                         Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_NSTextView_setBackgroundColor(TextView *txt, double red, double green,
                                      double blue, double alpha) {
  [txt setBackgroundColor:ml_NSColor_make(red, green, blue, alpha)];
}

CAMLprim value ml_NSTextView_setBackgroundColor_bc(TextView *txt, value red_v,
                                                   value green_v, value blue_v,
                                                   value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_NSTextView_setBackgroundColor(txt, Double_val(red_v), Double_val(blue_v),
                                   Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}
