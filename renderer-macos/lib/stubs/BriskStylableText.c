#import "BriskStylableText.h"

void ml_BriskStylableText_applyChanges(id<BriskStylableText> txt) {
  [txt applyTextStyle];
}

CAMLprim value ml_BriskStylableText_setFont(id<BriskStylableText> txt,
                                            value fontName_v, double fontSize,
                                            double fontWeight) {
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

  CAMLreturn(Val_unit);
}

void ml_BriskStylableText_setColor(id<BriskStylableText> txt, double red,
                                   double green, double blue, double alpha) {
  txt.attributedProps[NSForegroundColorAttributeName] =
      [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
}

CAMLprim value ml_BriskStylableText_setColor_bc(id<BriskStylableText> txt,
                                                value red_v, value green_v,
                                                value blue_v, value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskStylableText_setColor(txt, Double_val(red_v), Double_val(blue_v),
                                Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskStylableText_setAlignment(id<BriskStylableText> txt, int align) {
  txt.paragraphStyle.alignment = align;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;
}

CAMLprim value ml_BriskStylableText_setAlignment_bc(id<BriskStylableText> txt,
                                                    value align_v) {
  CAMLparam1(align_v);

  ml_BriskStylableText_setAlignment(txt, Int_val(align_v));

  CAMLreturn(Val_unit);
}

void ml_BriskStylableText_setLineBreakMode(id<BriskStylableText> txt,
                                           int mode) {
  txt.paragraphStyle.lineBreakMode = mode;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;
}

CAMLprim value ml_BriskStylableText_setLineBreakMode_bc(
    id<BriskStylableText> txt, value mode_v) {
  CAMLparam1(mode_v);

  ml_BriskStylableText_setLineBreakMode(txt, Int_val(mode_v));

  CAMLreturn(Val_unit);
}

void ml_BriskStylableText_setLineSpacing(id<BriskStylableText> txt,
                                         double spacing) {
  txt.paragraphStyle.lineSpacing = spacing;
  txt.attributedProps[NSParagraphStyleAttributeName] = txt.paragraphStyle;
}

CAMLprim value ml_BriskStylableText_setLineSpacing_bc(id<BriskStylableText> txt,
                                                      value spacing_v) {
  CAMLparam1(spacing_v);

  ml_BriskStylableText_setLineSpacing(txt, Double_val(spacing_v));

  CAMLreturn(Val_unit);
}

void ml_BriskStylableText_setKern(id<BriskStylableText> txt, double kern) {
  txt.attributedProps[NSKernAttributeName] = [NSNumber numberWithDouble:kern];
}

CAMLprim value ml_BriskStylableText_setKern_bc(id<BriskStylableText> txt,
                                               value kern_v) {
  CAMLparam1(kern_v);

  ml_BriskStylableText_setKern(txt, Double_val(kern_v));

  CAMLreturn(Val_unit);
}
