#import "BriskWindowDelegate.h"
#import <caml/callback.h>
#import <caml/memory.h>

@implementation BriskWindowDelegate

- (void)windowDidResize:(NSNotification *)__unused aNotification {
  if (self.didResizeCallback) {
    brisk_caml_call_and_flush(^{
      caml_callback(self.didResizeCallback, Val_unit);
    });
  }
}

- (void)setOnWindowDidResize:(value)callback {
  if (self.didResizeCallback) {
    value prevCallback = self.didResizeCallback;
    caml_remove_global_root(&prevCallback);
  }
  caml_register_global_root(&callback);
  self.didResizeCallback = callback;
}

@end
