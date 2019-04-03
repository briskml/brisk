#import "BriskCocoa.h"

@interface NSObject (BriskOCamlClosureEventTarget)
@property(nonatomic, weak) id target;
@property(nonatomic, assign) SEL action;
@property(nonatomic, strong, readonly) id briskOCamlClosureTarget;
- (void)brisk_unsafe_setClosureTarget:(value)callbackOption;
@end

void ml_setOCamlClosureTarget(NSObject *object, value callbackOption);
