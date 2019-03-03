#import "BriskCocoa.h"

@interface BriskClickable : NSView

@property(nonatomic, retain) NSClickGestureRecognizer *handler;
@property(nonatomic, assign) value _onClick;

- (void)setOnClick:(value)callback;

@end

@implementation BriskClickable

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
  if (self._onClick) {
    value prevCallback = self._onClick;
    caml_remove_global_root(&prevCallback);
  }
  caml_register_global_root(&callback);
  self._onClick = callback;

  self.handler.target = self;
  self.handler.numberOfClicksRequired = 1;
  self.handler.action = @selector(performClick);
}

- (void)performClick {
  brisk_caml_call(self._onClick);
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
