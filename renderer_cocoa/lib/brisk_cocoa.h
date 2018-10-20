#ifndef BRISK_COCOA_H
#define BRISK_COCOA_H

#define CAML_NAME_SPACE

#include <Cocoa/Cocoa.h>

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

// OCaml-specific
#define Val_none Val_int(0)

value Val_some(value some_v);

#define Some_val(v) Field(v, 0)

#define Val_NSApplication(v) ((value)(v))
#define NSApplication_val(v) ((__bridge NSApplication *)(value)(v))

#define Val_NSWindow(v) ((value)(v))
#define NSWindow_val(v) ((__bridge NSWindow *)(value)(v))

#define Val_View(v) ((value)(v))
#define View_val(v) ((__bridge View *)(value)(v))

#define Val_Button(v) ((value)(v))
#define Button_val(v) ((__bridge Button *)(value)(v))

CAMLprim value ml_NSLog(value str);

// Cocoa-specific

typedef void (^Callback)();

@interface View : NSView
@end

@interface Button : NSButton

@property(nonatomic, copy) Callback _callback;

- (void)onClick:(Callback)action;
@end

@interface MLApplicationDelegate : NSObject <NSApplicationDelegate>

@end

enum { ApplicationWillFinishLaunching, ApplicationDidFinishLaunching };

@interface MLWindowDelegate : NSObject <NSWindowDelegate>

@end

enum { WindowDidResize };

#endif /* BRISK_COCOA_H */
