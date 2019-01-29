#import "BriskWindowDelegate.h"

@implementation BriskWindowDelegate

- (void)windowDidResize:(NSNotification *)__unused aNotification {
  if (self.didResizeCallback) {
    brisk_caml_call(self.didResizeCallback);
  }
}

- (void)setOnWindowDidResize:(value)callback {
  value callback_f = callback;
  if (self.didResizeCallback) {
    value prevCallback = self.didResizeCallback;
    caml_modify_generational_global_root(&prevCallback, callback_f);
  }
  caml_register_generational_global_root(&callback_f);
  self.didResizeCallback = callback_f;
}

@end
