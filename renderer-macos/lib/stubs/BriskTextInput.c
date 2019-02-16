@interface BriskTextInput : NSTextInputContext <NSTextInputControl>

@end

@implementation NSTextInputContext
- (id)init {
  self = [super init];
  return self;
}
@property(nonatomic, assign) value _callback;

- (void)setCallback:(value)action;
@end

NSTextInputContext *ml_BriskTextInput_make() {
  BriskTextInput *input = [BriskTextInput new];
  retainView(input);

  return input;
}

CAMLprim value ml_BriskTextInput_setCallback(BriskTextInput *input, value callback_v) {
  CAMLparam1(callback_v);
  value callback = callback_v;

  [input setCallback:callback];

  CAMLreturn(Val_unit);
}
