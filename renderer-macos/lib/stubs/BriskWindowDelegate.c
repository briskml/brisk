#import "BriskWindowDelegate.h"

@implementation BriskWindowDelegate {
  value didResizeCallback;
}

- (void)windowDidResize:(NSNotification *)__unused aNotification {
  if (didResizeCallback) {
    brisk_caml_call(didResizeCallback);
  }
}

- (void)setOnWindowDidResize:(value)callback {
  if (didResizeCallback) {
    caml_modify_generational_global_root(&didResizeCallback, callback);
  } else {
    didResizeCallback = callback;
    caml_register_generational_global_root(&didResizeCallback);
  }
}

@end
