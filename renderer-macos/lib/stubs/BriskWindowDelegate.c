#import "BriskWindowDelegate.h"

@implementation BriskWindowDelegate

- (void)windowDidResize:(NSNotification *)__unused aNotification {
  if (self.didResizeCallback) {
    brisk_caml_call(self.didResizeCallback);
  }
}

- (void)setOnWindowDidResize:(value)callback {
  if (self.didResizeCallback) {
    value prevCallback = self.didResizeCallback;
    caml_modify_generational_global_root(&prevCallback, callback);
  }
  caml_register_generational_global_root(&callback);
  self.didResizeCallback = callback;
}

@end
