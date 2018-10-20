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
