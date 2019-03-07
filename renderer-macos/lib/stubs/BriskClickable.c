#import "BriskCocoa.h"

@interface BriskClickable : NSView

@property(nonatomic, retain) NSClickGestureRecognizer *handler;

- (void)setOnClick:(value)callback;

@end

@implementation BriskClickable {
  value onClick;
}

- (id)init {
  self = [super init];
  if (self) {
    self.handler = [[NSClickGestureRecognizer alloc] init];
    [self addGestureRecognizer:self.handler];
  }
  return self;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)__unused ev {
  return YES;
}

- (void)setOnClick:(value)callback {

  if (onClick) {
    caml_modify_generational_global_root(&onClick, callback);
  } else {
    onClick = callback;
    caml_register_generational_global_root(&onClick);
  }

  self.handler.target = self;
  self.handler.numberOfClicksRequired = 1;
  self.handler.action = @selector(performClick);
}

- (void)performClick {
  if (onClick) {
    brisk_caml_call(onClick);
  }
}

@end

BriskClickable *ml_BriskClickable_make() {
  BriskClickable *btn = [BriskClickable new];
  retainView(btn);

  return btn;
}

CAMLprim value ml_BriskClickable_setOnClick(BriskClickable *btn,
                                            value callback_v) {
  CAMLparam1(callback_v);
  value callback = callback_v;

  [btn setOnClick:callback];

  CAMLreturn(Val_unit);
}
