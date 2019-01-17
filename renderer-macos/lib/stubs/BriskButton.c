#import "BriskCocoa.h"
#import "BriskStylableText.h"

@interface BriskButton : NSButton <BriskStylableText>

@property(nonatomic, assign) value _callback;

- (void)setCallback:(value)action;

@end

@implementation BriskButton

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

- (void)applyTextStyle {
  NSRange range = NSMakeRange(0, self.attributedString.length);
  [self.attributedString setAttributes:self.attributedProps range:range];
  [self setAttributedTitle:self.attributedString];
}

- (void)setCallback:(value)callback {
  if (self._callback) {
    value prevCallback = self._callback;
    caml_remove_global_root(&prevCallback);
  }
  caml_register_global_root(&callback);
  self._callback = callback;
  [self setTarget:self];
  [self setAction:@selector(performCallback)];
}

- (void)performCallback {
  brisk_caml_call(^{
    caml_callback(self._callback, Val_unit);
  });
}
@end

BriskButton *ml_BriskButton_make() {
  BriskButton *btn = [BriskButton new];
  retainView(btn);

  return btn;
}

CAMLprim value ml_BriskButton_setCallback(BriskButton *btn, value callback_v) {
  CAMLparam1(callback_v);
  value callback = callback_v;

  [btn setCallback:callback];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskButton_setTitle(BriskButton *btn, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn.attributedString.mutableString setString:str];

  CAMLreturn(Val_unit);
}

// CAMLprim value ml_BriskButton_setTitle(BriskButton *btn, value str_v) {
//   CAMLparam1(str_v);

//   NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
//   [btn setTitle:str];

//   CAMLreturn(Val_unit);
// }

void ml_BriskButton_setButtonType(BriskButton *btn, intnat type) {
  [btn setButtonType:type];
}

CAMLprim value ml_BriskButton_setButtonType_bc(BriskButton *btn, value type_v) {
  CAMLparam1(type_v);

  ml_BriskButton_setButtonType(btn, Int_val(type_v));
  CAMLreturn(Val_unit);
}

void ml_BriskButton_setBezelStyle(BriskButton *btn, intnat bezel) {
  [btn setBezelStyle:bezel];
}

CAMLprim value ml_BriskButton_setBezelStyle_bc(BriskButton *btn,
                                               value bezel_v) {
  CAMLparam1(bezel_v);

  ml_BriskButton_setBezelStyle(btn, Int_val(bezel_v));
  CAMLreturn(Val_unit);
}

void ml_BriskButton_setIsBordered(BriskButton *btn, intnat bordered) {
  [btn setBordered:bordered];
}

CAMLprim value ml_BriskButton_setIsBordered_bc(BriskButton *btn,
                                               value bordered_v) {
  CAMLparam1(bordered_v);

  ml_BriskButton_setIsBordered(btn, Int_val(bordered_v));
  CAMLreturn(Val_unit);
}
