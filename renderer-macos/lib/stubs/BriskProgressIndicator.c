#import "BriskCocoa.h"

NSProgressIndicator *ml_BriskProgressIndicator_make() {
    NSProgressIndicator *progressIndicator = [NSProgressIndicator new];
    [progressIndicator startAnimation:nil];
    progressIndicator.indeterminate = YES;
    retainView(progressIndicator);
    return progressIndicator;
}
