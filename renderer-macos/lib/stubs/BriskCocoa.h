#define CAML_NAME_SPACE
#import <Cocoa/Cocoa.h>
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

#define ACCESSOR(VALUE)                                                        \
  intnat ml_get##VALUE() { return VALUE; }                                     \
  value ml_get##VALUE##_bc() {                                                 \
    CAMLparam0();                                                              \
    CAMLreturn(VALUE);                                                         \
  }

// Memoize registered OCaml callbacks
void brisk_caml_memoize(const char *name, value **staticPointer);

// Enter OCaml runtime and obtain the semaphore
void brisk_caml_call(value callback);

// Manual memory management of NSViews
void retainView(NSView *view);
void releaseView(NSView *view);
