#import "BriskApplicationDelegate.h"
#import "BriskCocoa.h"

@implementation BriskApplicationDelegate

- (void)callOCamlFunctionNamed:(const char *)name
              memoizeToPointer:(value **)staticPointer {
  if (*staticPointer == NULL) {
    *staticPointer = caml_named_value(name);
  }

  if (*staticPointer != NULL) {
    caml_callback(**staticPointer, Val_unit);
  }
}

- (void)applicationWillTerminate:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  brisk_caml_call(^{
    [self callOCamlFunctionNamed:"NSAppDelegate.applicationWillTerminate"
                memoizeToPointer:&closure_f];
  });
}

- (void)applicationWillFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  brisk_caml_call(^{
    [self callOCamlFunctionNamed:"NSAppDelegate.applicationWillFinishLaunching"
                memoizeToPointer:&closure_f];
  });
}

- (void)applicationDidFinishLaunching:(NSNotification *)__unused not{
  static value *closure_f = NULL;
  brisk_caml_call(^{
    [self callOCamlFunctionNamed:"NSAppDelegate.applicationDidFinishLaunching"
                memoizeToPointer:&closure_f];
  });
}

@end
