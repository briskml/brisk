#import "Cocoa_helpers_stubs.h"

NSColor *ml_NSColor_make(double red, double green, double blue, double alpha) {
  return [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
}
