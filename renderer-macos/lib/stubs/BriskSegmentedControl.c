#import "BriskCocoa.h"

@interface BriskSegmentedControlItem : NSObject
@property(nonatomic) BOOL isSelected;
@property(nonatomic, strong) NSString *text;
@end
@implementation BriskSegmentedControlItem {
    value onClick;
}

- (void)setOnClick:(value)callback {
    [self willChangeValueForKey:@"onClick"];
    if (onClick) {
        caml_modify_generational_global_root(&onClick, callback);
    } else {
        onClick = callback;
        caml_register_generational_global_root(&onClick);
    }
    [self didChangeValueForKey:@"onClick"];
}

- (value)onClick {
    return onClick;
}

@end

BriskSegmentedControlItem *ml_NSSegmentedControl_Item_make() {
    BriskSegmentedControlItem *item = [BriskSegmentedControlItem new];
    retainView((id)item);
    return item;
}

CAMLprim value ml_NSSegmentedControl_Item_setText(
    BriskSegmentedControlItem *item, value text) {
    CAMLparam1(text);

    NSString *str = [NSString stringWithUTF8String:String_val(text)];
    item.text = str;

    CAMLreturn(Val_unit);
}

void ml_NSSegmentedControl_Item_setSelected(BriskSegmentedControlItem *item,
                                            intnat isSelected) {
    item.isSelected = (BOOL)isSelected;
}

CAMLprim value ml_NSSegmentedControl_Item_setOnClick(
    BriskSegmentedControlItem *item, value callback) {
    CAMLparam1(callback);
    [item setOnClick:callback];
    CAMLreturn(Val_unit);
}

NSSegmentedControl *ml_NSSegmentedControl_make() {
    NSSegmentedControl *control = [NSSegmentedControl
        segmentedControlWithLabels:@[]
                      trackingMode:NSSegmentSwitchTrackingSelectOne
                            target:nil
                            action:nil];
    retainView(control);
    return control;
}

void ml_NSSegmentedControl_insertItem(NSSegmentedControl *segmentedControl,
                                      BriskSegmentedControlItem *item,
                                      intnat position) {
    NSMutableArray *labels = [NSMutableArray new];
    for (int i = 0; i < segmentedControl.segmentCount; i++) {
        [labels addObject:[segmentedControl labelForSegment:i]];
    }
    [labels insertObject:item.text atIndex:position];
    segmentedControl.segmentCount += 1;
    for (int i = 0; i < segmentedControl.segmentCount; i++) {
        [segmentedControl setLabel:labels[i] forSegment:i];
    }
}

void ml_NSSegmentedControl_deleteItem(
    NSSegmentedControl __unused *segmentedControl,
    BriskSegmentedControlItem __unused *item) {
    NSCAssert(NO, @"TODO: implement deleteItem");
}
