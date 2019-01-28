#import "BriskApplicationDelegate.h"
#import "BriskCocoa.h"
#import <caml/threads.h>

void ml_NSApplication_init() {
  NSApplication *app = [NSApplication sharedApplication];
  [app setActivationPolicy:NSApplicationActivationPolicyRegular];
  [app activateIgnoringOtherApps:YES];
  [app setDelegate:[BriskApplicationDelegate new]];
}

void ml_NSApplication_run() {
  caml_release_runtime_system();
  @autoreleasepool {
    [[NSApplication sharedApplication] run];
  }
}
