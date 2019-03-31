#import "BriskCocoa.h"

NSProgressIndicator *ml_BriskProgressIndicator_make() {
  NSProgressIndicator *progressIndicator = [NSProgressIndicator new];
  progressIndicator.style = NSProgressIndicatorStyleSpinning;
  [progressIndicator startAnimation:nil];
  progressIndicator.indeterminate = YES;
  retainView(progressIndicator);
  return progressIndicator;
}

double ml_BriskProgressIndicator_fittingWidth(NSProgressIndicator *indicator) {
  return indicator.fittingSize.width;
}

double ml_BriskProgressIndicator_fittingHeight(NSProgressIndicator *indicator) {
  return indicator.fittingSize.height;
}
