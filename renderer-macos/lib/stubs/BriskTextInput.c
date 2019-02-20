@interface BriskTextInput : NSTextInputContext <NSTextInputClient>

@end

@implementation BriskTextInput

@end

NSTextInputContext *ml_BriskTextInput_make() {
  BriskTextInput *input = [BriskTextInput new];
  retainView(input);

  return input;
}

