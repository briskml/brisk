#import "OCamlClosureEventTarget.h"
#import <objc/runtime.h>

@implementation BriskOCamlClosureEventTarget {
  value callback;
}

- (void)setCallback:(value)callbackOption {
  if (callback) {
    if (Is_block(callbackOption)) {
      caml_modify_generational_global_root(&callback, Field(callbackOption, 0));
    } else {
      caml_remove_generational_global_root(&callback);
      callback = (value)NULL;
    }
  } else if (Is_block(callbackOption)) {
    callback = Field(callbackOption, 0);
    caml_register_generational_global_root(&callback);
  }
}

- (void)performCallback0 {
  if (callback) {
    brisk_caml_call(callback);
  }
}

- (void)performCallback1:(value)arg {
  if (callback) {
    value args[] = {arg};
    brisk_caml_call_n(callback, 1, args);
  }
}

@end

@implementation NSObject (BriskOCamlClosureEventTarget)
@dynamic action;
@dynamic target;
@dynamic briskOCamlClosureTarget;

- (BriskOCamlClosureEventTarget *)briskOCamlClosureTarget {
  BriskOCamlClosureEventTarget *target =
      objc_getAssociatedObject(self, @selector(briskOCamlClosureTarget));
  if (!target) {
    target = [BriskOCamlClosureEventTarget new];
    objc_setAssociatedObject(self, @selector(briskOCamlClosureTarget), target,
                             OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  return target;
}

- (void)brisk_unsafe_setClosureTarget:(value)callbackOption {
  [self.briskOCamlClosureTarget setCallback:callbackOption];
  if (callbackOption) {
    self.target = self.briskOCamlClosureTarget;
    self.action = @selector(performCallback0);
  } else {
    self.target = nil;
    self.action = nil;
  }
}
@end

void ml_setOCamlClosureTarget(NSObject *object, value callbackOption) {
  [object brisk_unsafe_setClosureTarget:callbackOption];
}
