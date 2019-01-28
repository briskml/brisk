#import "BriskApplicationDelegate.h"
#import "BriskCocoa.h"

@implementation BriskApplicationDelegate

- (void)applicationWillTerminate:(NSNotification *)__unused not{
  static value *callbackFn = NULL;
  brisk_caml_memoize("NSAppDelegate.applicationWillTerminate", &callbackFn);

  if (callbackFn != NULL) {
    brisk_caml_call(*callbackFn);
  }
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not{
  static value *callbackFn = NULL;
  brisk_caml_memoize("NSAppDelegate.applicationWillFinishLaunching",
                     &callbackFn);

  if (callbackFn != NULL) {
    brisk_caml_call(*callbackFn);
  }
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not{
  static value *callbackFn = NULL;
  brisk_caml_memoize("NSAppDelegate.applicationDidFinishLaunching",
                     &callbackFn);

  if (callbackFn != NULL) {
    brisk_caml_call(*callbackFn);
  }
}

@end
