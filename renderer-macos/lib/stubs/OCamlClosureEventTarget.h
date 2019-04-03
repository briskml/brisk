#import "BriskCocoa.h"

@interface NSObject (BriskOCamlClosureEventTarget)
@property(nonatomic, weak) id target;
@property(nonatomic, assign) SEL action;
@property(nonatomic, strong, readonly) id briskOCamlClosureTarget;
- (void)brisk_unsafe_setClosureTarget:(value)callbackOption;
@end

@interface BriskOCamlClosureEventTarget : NSObject
- (void)setCallback:(value)callbackOption;
- (void)performCallback0;
- (void)performCallback1:(value)arg;
@end

void ml_setOCamlClosureTarget(NSObject *object, value callbackOption);
