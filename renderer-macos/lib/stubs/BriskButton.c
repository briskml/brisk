#import "BriskImageHostingView.h"
#import "BriskStylableText.h"

@interface BriskButton : NSButton <BriskStylableText, BriskImageHostingView>

@property(nonatomic, strong) NSMutableAttributedString *brisk_attributedString;
@property(nonatomic, strong) NSMutableDictionary *brisk_textAttributes;

- (void)setCallback:(value)action;

@end

@implementation BriskButton {
  value onClick;
}

@synthesize brisk_paragraphStyle;

- (id)init {
  self = [super init];

  if (self) {
    self.brisk_attributedString = [[NSMutableAttributedString alloc] init];
    self.brisk_textAttributes = [NSMutableDictionary dictionary];
    self.brisk_paragraphStyle =
        [[NSParagraphStyle defaultParagraphStyle] mutableCopy];

    self.brisk_textAttributes[NSParagraphStyleAttributeName] =
        self.brisk_paragraphStyle;
  }

  return self;
}

#pragma mark - BriskStylableText

- (void)brisk_setLineBreakMode:(NSLineBreakMode)mode {
  self.lineBreakMode = mode;
}

- (void)brisk_addAttribute:(NSAttributedStringKey)key value:(id)value {
  [self.brisk_attributedString
      addAttribute:key
             value:value
             range:NSMakeRange(0, self.brisk_attributedString.length)];
}

- (void)brisk_applyTextStyle {
  NSRange range = NSMakeRange(0, self.brisk_attributedString.length);
  [self.brisk_attributedString setAttributes:self.brisk_textAttributes
                                       range:range];
  [self setAttributedTitle:self.brisk_attributedString];
}

- (void)setCallback:(value)callback {
  if (onClick) {
    caml_modify_generational_global_root(&onClick, callback);
  } else {
    onClick = callback;
    caml_register_generational_global_root(&onClick);
  }

  [self setTarget:self];
  [self setAction:@selector(performCallback)];
}

- (void)performCallback {
  brisk_caml_call(onClick);
}

- (void)brisk_setImage:(NSImage *)image {
  self.image = image;
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
  [btn.brisk_attributedString.mutableString setString:str];

  CAMLreturn(Val_unit);
}

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

void ml_BriskButton_setIsBordered(BriskButton *btn, intnat bordered) {
  btn.bordered = bordered;
}

CAMLprim value ml_BriskButton_setIsBordered_bc(BriskButton *btn,
                                               value bordered_v) {
  CAMLparam1(bordered_v);

  ml_BriskButton_setIsBordered(btn, Int_val(bordered_v));
  CAMLreturn(Val_unit);
}
