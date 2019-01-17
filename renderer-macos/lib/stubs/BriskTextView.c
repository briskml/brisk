#import "BriskCocoa.h"
#import "BriskStylableText.h"

@interface BriskTextView : NSTextView <BriskStylableText>

@end

@implementation BriskTextView

@synthesize attributedString;
@synthesize attributedProps;
@synthesize paragraphStyle;

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

- (void)applyTextStyle {
  NSRange range = NSMakeRange(0, self.attributedString.length);

  [self.attributedString setAttributes:self.attributedProps range:range];
  [[self textStorage] setAttributedString:self.attributedString];
}

@end

BriskTextView *ml_BriskTextView_make() {
  BriskTextView *txt = [BriskTextView new];
  retainView(txt);

  return txt;
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
  [txt applyTextStyle];

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setBackgroundColor(BriskTextView *txt, double red,
                                         double green, double blue,
                                         double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
  [txt setBackgroundColor:color];
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

void ml_BriskTextView_setCornerRadius(BriskTextView *txt, double radius) {
  [txt setWantsLayer:YES];
  [txt.layer setCornerRadius:(CGFloat)radius];
}

CAMLprim value ml_BriskTextView_setCornerRadius_bc(BriskTextView *txt,
                                                   value radius_v) {
  CAMLparam1(radius_v);

  ml_BriskTextView_setCornerRadius(txt, Double_val(radius_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setPadding(BriskTextView *txt, double left, double top,
                                 __unused double right,
                                 __unused double bottom) {
  txt.textContainerInset = CGSizeMake(left, top);
}

CAMLprim value ml_BriskTextView_setPadding_bc(BriskTextView *txt, value left_v,
                                              value top_v, value right_v,
                                              value bottom_v) {
  CAMLparam4(left_v, top_v, right_v, bottom_v);

  ml_BriskTextView_setPadding(txt, Double_val(left_v), Double_val(top_v),
                              Double_val(right_v), Double_val(bottom_v));

  CAMLreturn(Val_unit);
}
