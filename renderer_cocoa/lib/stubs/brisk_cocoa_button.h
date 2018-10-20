#ifndef BRISK_COCOA_BUTTON_H
#define BRISK_COCOA_BUTTON_H

#define CAML_NAME_SPACE

#include "brisk_cocoa.h"

#define Val_Button(v) ((value)(v))
#define Button_val(v) ((__bridge Button *)(value)(v))

@interface Button : NSButton

@property(nonatomic, copy) Callback _callback;

- (void)onClick:(Callback)action;
@end

#endif /* BRISK_COCOA_BUTTON_H */
