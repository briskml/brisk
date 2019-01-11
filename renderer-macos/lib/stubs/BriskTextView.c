#import "BriskTextView.h"
#define CAML_NAME_SPACE
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

@implementation BriskTextView

- (id)init {
  self = [super init];
  if (self) {
    self.attributedString = [[NSMutableAttributedString alloc] init];
    self.attributedProps = [NSMutableDictionary dictionary];
    self.paragraphStyle =
        [[NSParagraphStyle defaultParagraphStyle] mutableCopy];

    self.attributedProps[NSParagraphStyleAttributeName] = self.paragraphStyle;
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

BriskTextView *ml_BriskTextView_make() {
  BriskTextView *txt = [BriskTextView new];
  retainView(txt);

  return txt;
}

void ml_BriskTextView_applyAttributes(BriskTextView *txt) {
  NSRange range = NSMakeRange(0, txt.attributedString.length);

  [txt.attributedString setAttributes:txt.attributedProps range:range];
  [[txt textStorage] setAttributedString:txt.attributedString];
}

double ml_BriskTextView_getTextWidth(BriskTextView *txt) {
  return (double)[txt.attributedString size].width;
}

double ml_BriskTextView_getTextHeight(BriskTextView *txt) {
  return (double)[txt.attributedString size].height;
}

CAMLprim value ml_BriskTextView_getTextWidth_bc(BriskTextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextView_getTextWidth(txt)));
}

CAMLprim value ml_BriskTextView_getTextHeight_bc(BriskTextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextView_getTextHeight(txt)));
}

CAMLprim value ml_BriskTextView_setStringValue(BriskTextView *txt,
                                               value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];

  [txt.attributedString.mutableString setString:str];

  ml_BriskTextView_applyAttributes(txt);

  CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskTextView_setFont(BriskTextView *txt, value fontName_v,
                                        double fontSize, double fontWeight) {
  CAMLparam1(fontName_v);

  NSString *fontName = [NSString stringWithUTF8String:String_val(fontName_v)];
  NSFont *font;

  if (fontName.length > 0) {
    NSDictionary *attributes = @{
      NSFontFaceAttribute : fontName,
      NSFontTraitsAttribute :
          @{NSFontWeightTrait : [NSNumber numberWithDouble:fontWeight]}
    };

    NSFontDescriptor *descriptor =
        [NSFontDescriptor fontDescriptorWithFontAttributes:attributes];

    font = [NSFont fontWithDescriptor:descriptor size:(CGFloat)fontSize];
  } else {
    font = [NSFont systemFontOfSize:(CGFloat)fontSize weight:fontWeight];
  }

  txt.attributedProps[NSFontAttributeName] = font;

  ml_BriskTextView_applyAttributes(txt);

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setColor(BriskTextView *txt, double red, double green,
                               double blue, double alpha) {
  txt.attributedProps[NSForegroundColorAttributeName] =
      ml_NSColor_make(red, green, blue, alpha);

  ml_BriskTextView_applyAttributes(txt);
}

CAMLprim value ml_BriskTextView_setColor_bc(BriskTextView *txt, value red_v,
                                            value green_v, value blue_v,
                                            value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskTextView_setColor(txt, Double_val(red_v), Double_val(blue_v),
                            Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setBackgroundColor(BriskTextView *txt, double red,
                                         double green, double blue,
                                         double alpha) {
  [txt setBackgroundColor:ml_NSColor_make(red, green, blue, alpha)];
}

CAMLprim value ml_BriskTextView_setBackgroundColor_bc(BriskTextView *txt,
                                                      value red_v,
                                                      value green_v,
                                                      value blue_v,
                                                      value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskTextView_setBackgroundColor(txt, Double_val(red_v),
                                      Double_val(blue_v), Double_val(green_v),
                                      Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setAlignment(BriskTextView *txt, int align) {
  txt.paragraphStyle.alignment = align;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;

  ml_BriskTextView_applyAttributes(txt);
}

CAMLprim value ml_BriskTextView_setAlignment_bc(BriskTextView *txt,
                                                value align_v) {
  CAMLparam1(align_v);

  ml_BriskTextView_setAlignment(txt, Int_val(align_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setLineBreakMode(BriskTextView *txt, int mode) {
  txt.paragraphStyle.lineBreakMode = mode;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;

  ml_BriskTextView_applyAttributes(txt);
}

CAMLprim value ml_BriskTextView_setLineBreakMode_bc(BriskTextView *txt,
                                                    value mode_v) {
  CAMLparam1(mode_v);

  ml_BriskTextView_setLineBreakMode(txt, Int_val(mode_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setLineSpacing(BriskTextView *txt, double spacing) {
  txt.paragraphStyle.lineSpacing = spacing;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;

  ml_BriskTextView_applyAttributes(txt);
}

CAMLprim value ml_BriskTextView_setLineSpacing_bc(BriskTextView *txt,
                                                  value spacing_v) {
  CAMLparam1(spacing_v);

  ml_BriskTextView_setLineSpacing(txt, Double_val(spacing_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setKern(BriskTextView *txt, double kern) {
  txt.attributedProps[NSKernAttributeName] = [NSNumber numberWithDouble:kern];

  ml_BriskTextView_applyAttributes(txt);
}

CAMLprim value ml_BriskTextView_setKern_bc(BriskTextView *txt, value kern_v) {
  CAMLparam1(kern_v);

  ml_BriskTextView_setKern(txt, Double_val(kern_v));

  CAMLreturn(Val_unit);
}
