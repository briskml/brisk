#import "BriskApplicationDelegate.h"
#import "BriskCocoa.h"

@implementation BriskApplicationDelegate

- (value *)memoizeOCamlFunctionNamed:(const char *)name
              toPointer:(value **)staticPointer {
  if (*staticPointer == NULL) {
    *staticPointer = caml_named_value(name);
  }
  return *staticPointer;
}

- (void)applicationWillTerminate:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  closure_f = [self memoizeOCamlFunctionNamed:"NSAppDelegate.applicationWillTerminate"
                toPointer:&closure_f];

  brisk_caml_call(*closure_f);
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  closure_f = [self memoizeOCamlFunctionNamed:"NSAppDelegate.applicationWillFinishLaunching"
                toPointer:&closure_f];

  brisk_caml_call(*closure_f);
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  closure_f = [self memoizeOCamlFunctionNamed:"NSAppDelegate.applicationDidFinishLaunching"
                toPointer:&closure_f];

  brisk_caml_call(*closure_f);
}

@end
