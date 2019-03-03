#import "BriskCocoa.h"
#import "BriskApplicationDelegate.h"
#import "BriskWindowDelegate.h"
#import <caml/threads.h>

void brisk_caml_memoize(const char *name, value **staticPointer) {
  if (*staticPointer == NULL) {
    *staticPointer = caml_named_value(name);
  }
}

void brisk_caml_call_n(value f, int argCount, value *args) {
  caml_c_thread_register();
  caml_acquire_runtime_system();
  caml_callbackN(f, argCount, args);
  caml_release_runtime_system();
}

void brisk_caml_call(value f) {
  caml_c_thread_register();
  caml_acquire_runtime_system();
  caml_callback(f, Val_unit);
  caml_release_runtime_system();
}

void retainView(NSView *view) { [view retain]; }

void releaseView(NSView *view) { [view release]; }
