#ifndef BRISK_COCOA_H
#define BRISK_COCOA_H

#define CAML_NAME_SPACE

#import <Cocoa/Cocoa.h>

#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>
#import <caml/mlvalues.h>
#import <caml/threads.h>

// OCaml-specific
#define Val_none Val_int(0)

value Val_some(value some_v);

#define Some_val(v) Field(v, 0)

#define Val_NSApplication(v) ((value)(v))
#define NSApplication_val(v) ((__bridge NSApplication *)(value)(v))

#define Val_NSWindow(v) ((value)(v))
#define NSWindow_val(v) ((__bridge NSWindow *)(value)(v))

CAMLprim value ml_NSLog(value str);

extern NSMutableDictionary *ml_Views;
extern NSMutableArray *ml_Views_all;

// Cocoa-specific

typedef void (^Callback)();

#endif /* BRISK_COCOA_H */
