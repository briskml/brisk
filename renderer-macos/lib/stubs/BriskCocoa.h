#define CAML_NAME_SPACE
#import <Cocoa/Cocoa.h>
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

// Memoize registered OCaml callbacks
void brisk_caml_memoize(const char *name, value **staticPointer);

void brisk_caml_call(value callback);

void brisk_caml_call_n(value f, int argCount, value *args);

// Manual memory management of NSViews
void retainView(NSView *view);
void releaseView(NSView *view);
