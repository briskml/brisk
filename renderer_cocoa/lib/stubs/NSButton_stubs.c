#include "brisk_cocoa_button.h"

@implementation Button

- (void)onClick:(Callback)action {
  self._callback = action;
  [self setTarget:self];
  [self setAction:@selector(performCallback:)];
}

- (void)performCallback:(id)__unused sender {
  self._callback();
}
@end

Button *ml_NSButton_make() {
  Button *btn = [Button new];
  [ml_Views_all addObject:btn];

  return btn;
}

CAMLprim value ml_NSButton_setCallback(Button *btn, value callback_v) {
  CAMLparam1(callback_v);

  value callback = callback_v;

  caml_register_global_root(&callback);

  [btn onClick:^{
    caml_callback(callback, Val_unit);
  }];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_NSButton_setTitle(Button *btn, value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];
  [btn setTitle:str];

  CAMLreturn(Val_unit);
}

void ml_NSButton_setButtonType(Button *btn, intnat type) {
  [btn setButtonType:type];
}

CAMLprim value ml_NSButton_setButtonType_bc(Button *btn, value type_v) {
  CAMLparam1(type_v);

  ml_NSButton_setButtonType(btn, Int_val(type_v));
  CAMLreturn(Val_unit);
}

void ml_NSButton_setBezelStyle(Button *btn, intnat bezel) {
  [btn setBezelStyle:bezel];
}

CAMLprim value ml_NSButton_setBezelStyle_bc(Button *btn, value bezel_v) {
  CAMLparam1(bezel_v);

  ml_NSButton_setBezelStyle(btn, Int_val(bezel_v));
  CAMLreturn(Val_unit);
}

void ml_NSButton_setIsBordered(Button *btn, BOOL bordered) {
  btn.bordered = bordered;
}

CAMLprim value ml_NSButton_setIsBordered_bc(Button *btn, value bordered_v) {
  CAMLparam1(bordered_v);

  ml_NSButton_setIsBordered(btn, Bool_val(bordered_v));
  CAMLreturn(Val_unit);
}
