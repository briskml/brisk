#import "NSTextView_stubs.h"
#define CAML_NAME_SPACE
#import <caml/alloc.h>
#import <caml/callback.h>
#import <caml/memory.h>

@implementation TextView

@end

TextView *ml_NSTextView_make() {
  TextView *txt = [TextView new];
  retainView(txt);

  return txt;
}
