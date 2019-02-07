#import "BriskCocoa.h"

#define DIVIDER_STYLE(VALUE) ACCESSOR(NSSplitViewDividerStyle##VALUE)

DIVIDER_STYLE(Thick)
DIVIDER_STYLE(Thin)
DIVIDER_STYLE(PaneSplitter)

@interface BriskSplitView : NSSplitView
@end

@implementation BriskSplitView
@end

BriskSplitView *ml_BriskSplitView_make() {
  BriskSplitView *split = [BriskSplitView new];
  retainView(split);

  return split;
}

void ml_BriskSplitView_setVertical(BriskSplitView *split, intnat vertical) {
  split.vertical = (BOOL)vertical;
}

CAMLprim value ml_BriskSplitView_setVertical_bc(BriskSplitView *split,
                                                value vertical_v) {
  CAMLparam1(vertical_v);
  ml_BriskSplitView_setVertical(split, Int_val(vertical_v));

  CAMLreturn(Val_unit);
}

void ml_BriskSplitView_setDividerStyle(BriskSplitView *split,
                                       NSSplitViewDividerStyle dividerStyle) {
  split.dividerStyle = dividerStyle;
}
