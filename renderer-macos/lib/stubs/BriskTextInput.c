#import "BriskTextInput.h"

@interface BriskTextInput : NSTextView <BriskTextInputProtocol>

@end

@implementation BriskTextInput

@synthesize attributedString;
@synthesize attributedProps;
@synthesize paragraphStyle;
@synthesize placeholder;

- (id)init {
  self = [super init];
  if (self) {
    self.attributedString = [[NSMutableAttributedString alloc] init];
    self.attributedProps = [NSMutableDictionary dictionary];
    self.paragraphStyle =
        [[NSParagraphStyle defaultParagraphStyle] mutableCopy];

    self.attributedProps[NSParagraphStyleAttributeName] = self.paragraphStyle;
    self.placeholder = [[NSMutableAttributedString alloc] init];
  }
  return self;
}

- (BOOL)isEditable {
  return YES;
}

- (BOOL)isSelectable {
  return YES;
}

- (void)applyTextStyle {
  NSRange range = NSMakeRange(0, self.attributedString.length);

  [self.attributedString setAttributes:self.attributedProps range:range];
  [[self textStorage] setAttributedString:self.attributedString];
}

@end

BriskTextInput *ml_BriskTextInput_make() {
  BriskTextInput *input = [BriskTextInput new];
  retainView(input);

  return input;
}

double ml_BriskTextInput_getTextWidth(BriskTextInput *input) {
  return (double)[input.attributedString size].width;
}

double ml_BriskTextInput_getTextHeight(BriskTextInput *input) {
  return (double)[input.attributedString size].height;
}

CAMLprim value ml_BriskTextInput_getTextWidth_bc(BriskTextInput *input) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextInput_getTextWidth(input)));
}

CAMLprim value ml_BriskTextInput_getTextHeight_bc(BriskTextInput *input) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextInput_getTextHeight(input)));
}

CAMLprim value ml_BriskTextInput_setStringValue(BriskTextInput *input,
                                               value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];

  [input.attributedString.mutableString setString:str];
  [input applyTextStyle];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskTextInput_setPlaceholder(BriskTextInput *input,
                                               value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];

  [input.placeholder.mutableString setString:str];
  [input applyTextStyle];

  CAMLreturn(Val_unit);
}

void ml_BriskTextInput_setBackgroundColor(BriskTextInput *input, double red,
                                         double green, double blue,
                                         double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
  [input setBackgroundColor:color];
}

CAMLprim value ml_BriskTextInput_setBackgroundColor_bc(BriskTextInput *input,
                                                      value red_v,
                                                      value green_v,
                                                      value blue_v,
                                                      value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskTextInput_setBackgroundColor(input, Double_val(red_v),
                                      Double_val(blue_v), Double_val(green_v),
                                      Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskTextInput_setPadding(BriskTextInput *input, double left, double top,
                                 __unused double right,
                                 __unused double bottom) {
  input.textContainerInset = CGSizeMake(left, top);
}

CAMLprim value ml_BriskTextInput_setPadding_bc(BriskTextInput *input, value left_v,
                                              value top_v, value right_v,
                                              value bottom_v) {
  CAMLparam4(left_v, top_v, right_v, bottom_v);

  ml_BriskTextInput_setPadding(input, Double_val(left_v), Double_val(top_v),
                              Double_val(right_v), Double_val(bottom_v));

  CAMLreturn(Val_unit);
}
