#import "BriskCocoa.h"
#import "BriskViewable.h"

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

@interface BriskSegmentedControl : NSObject <BriskView>
@property(nonatomic, strong) NSSegmentedControl *contentView;
@property(nonatomic, strong) NSMutableArray<BriskSegmentedControlItem *> *items;
@end

@implementation BriskSegmentedControl

- (instancetype)init {
    self = [super init];
    self.contentView = [NSSegmentedControl
        segmentedControlWithLabels:@[]
                      trackingMode:NSSegmentSwitchTrackingSelectOne
                            target:self
                            action:@selector(didSelect:)];
    self.items = [NSMutableArray new];
    return self;
}

- (void)didSelect:(NSSegmentedControl *)sender {
    value onClick = [self.items[sender.selectedSegment] onClick];
    [self.contentView setSelected:YES forSegment:sender.selectedSegment];
    if (onClick) {
        brisk_caml_call(onClick);
    }
}

- (void)insertItem:(BriskSegmentedControlItem *)item atIndex:(NSInteger)index {
    [self.items insertObject:item atIndex:index];
    [item addObserver:self
           forKeyPath:@"isSelected"
              options:NSKeyValueObservingOptionNew
              context:NULL];
    self.contentView.segmentCount += 1;
    for (int i = 0; i < self.contentView.segmentCount; i++) {
        [self.contentView setLabel:self.items[i].text forSegment:i];
        [self.contentView setWidth:0 forSegment:i];
        [self.contentView setSelected:self.items[i].isSelected forSegment:i];
    }
    releaseView((id)item);
}

- (void)observeValueForKeyPath:(NSString __unused *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary __unused *)change
                       context:(void __unused *)context {
    NSLog(@"%i", [object isSelected]);
    [self.contentView setSelected:[object isSelected]
                       forSegment:[self.items indexOfObject:object]];
}

- (NSView *)NSView {
    return self.contentView;
}

@end

BriskSegmentedControl *ml_NSSegmentedControl_make() {
    BriskSegmentedControl *control = [BriskSegmentedControl new];
    retainView((id)control);
    return control;
}

void ml_NSSegmentedControl_insertItem(BriskSegmentedControl *segmentedControl,
                                      BriskSegmentedControlItem *item,
                                      intnat position) {
    [segmentedControl insertItem:item atIndex:position];
}

void ml_NSSegmentedControl_deleteItem(
    NSSegmentedControl __unused *segmentedControl,
    BriskSegmentedControlItem __unused *item) {
    NSCAssert(NO, @"TODO: implement deleteItem");
}
