#import "BriskToolbar.h"
#import "BriskCocoa.h"
#import "BriskViewable.h"

@protocol BriskToolbarItem
- (NSToolbarItem *)generateToolbarItemForIdentifier:
    (NSToolbarItemIdentifier)identifier;
- (NSArray<NSToolbarItemIdentifier> *)identifiers;
@end

@interface BriskCustomToolbarItem : NSObject <BriskToolbarItem, BriskViewParent>
@property(nonatomic, strong)
    NSMutableDictionary<NSToolbarItemIdentifier, NSView *> *views;
@property(nonatomic, strong) NSMutableArray *identifiers;
@end
@implementation BriskCustomToolbarItem

- (instancetype)init {
  self = [super init];
  self.views = [NSMutableDictionary new];
  self.identifiers = [NSMutableArray new];
  return self;
}

#pragma mark - BriskToolbarItem

- (NSToolbarItem *)generateToolbarItemForIdentifier:
    (NSToolbarItemIdentifier)identifier {
  NSToolbarItem *toolbarItem =
      [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
  toolbarItem.view = NSViewFromBriskView(self.views[identifier]);
  toolbarItem.maxSize = toolbarItem.view.fittingSize;
  return toolbarItem;
}

#pragma mark - BriskViewParent

- (void)brisk_insertNode:(id)child position:(intnat)position {
  NSString *uuid = [[NSUUID new] UUIDString];
  /* TODO: This won't work if we just add another child to an item when the
   * toolbar is already displayed because we don't notify the toolbar itself
   * about the change. The whole logic here is very fragile though, so we'll
   * change it in follow up work.
   */
  [self.identifiers insertObject:uuid atIndex:position];
  [self.views setObject:child forKey:uuid];
}

- (void)brisk_deleteNode:(id)child {
  NSString *uuid = [[self.views allKeysForObject:child] firstObject];

  if (uuid) {
    [self.identifiers removeObject:uuid];
    [self.views removeObjectForKey:uuid];
  }
}

@end

BriskCustomToolbarItem *ml_NSToolbarItem_make() {
  BriskCustomToolbarItem *item = [BriskCustomToolbarItem new];
  retainView((id)item);
  return item;
}

@interface BriskFlexibleSpaceToolbarItem : NSObject <BriskToolbarItem>
@end
@implementation BriskFlexibleSpaceToolbarItem
- (NSToolbarItem *)generateToolbarItemForIdentifier:
    (NSToolbarItemIdentifier __unused)identifier {
  return [[NSToolbarItem alloc]
      initWithItemIdentifier:NSToolbarFlexibleSpaceItemIdentifier];
}

- (NSArray *)identifiers {
  return @[ NSToolbarFlexibleSpaceItemIdentifier ];
}
@end

BriskFlexibleSpaceToolbarItem *ml_NSToolbarItem_makeFlexibleSpace() {
  BriskFlexibleSpaceToolbarItem *item = [BriskFlexibleSpaceToolbarItem new];
  retainView((id)item);
  return item;
}

@interface BriskToolbar () <NSToolbarDelegate>
@property(nonatomic, strong) NSToolbar *toolbar;
@property(nonatomic, strong) NSMutableArray<id<BriskToolbarItem>> *items;
@end

@implementation BriskToolbar

- (BriskToolbar *)init {
  self = [super init];
  self.toolbar = [NSToolbar new];
  self.toolbar.delegate = self;
  self.items = [NSMutableArray new];
  return self;
}

- (void)insertItem:(id<BriskToolbarItem>)item index:(NSInteger)index {
  [self.items insertObject:item atIndex:index];
  /* Tracking lifetimes is getting trickier.
   * This item has been retained by something else.
   */
  for (NSUInteger i = 0; i < item.identifiers.count; i++) {
    NSToolbarItemIdentifier identifier = item.identifiers[i];
    [self.toolbar insertItemWithItemIdentifier:identifier atIndex:i + index];
  };
}

- (void)removeItem:(id<BriskToolbarItem>)item {
  NSInteger index = [self.items indexOfObject:item];
  [self.toolbar removeItemAtIndex:index];
  [self.items removeObjectAtIndex:index];
  releaseView((id)item);
}

- (NSToolbar *)NSToolbar {
  return self.toolbar;
}

#pragma mark - NSToolbarDelegate

- (NSToolbarItem *)toolbar:(NSToolbar __unused *)toolbar
        itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier
    willBeInsertedIntoToolbar:(BOOL __unused)flag {
  for (id<BriskToolbarItem> item in self.items) {
    if ([item.identifiers containsObject:itemIdentifier]) {
      return [item generateToolbarItemForIdentifier:itemIdentifier];
    }
  }
  return nil;
}

- (NSArray<NSToolbarItemIdentifier> *)toolbarDefaultItemIdentifiers:
    (NSToolbar __unused *)toolbar {
  return [self.items valueForKeyPath:@"@unionOfArrays.identifiers"];
}

- (NSArray<NSToolbarItemIdentifier> *)toolbarAllowedItemIdentifiers:
    (NSToolbar __unused *)toolbar {
  return [self toolbarDefaultItemIdentifiers:toolbar];
}

@end

BriskToolbar *ml_NSToolbar_make() {
  BriskToolbar *toolbar = [[BriskToolbar alloc] init];
  retainView((id)toolbar);
  return toolbar;
}

void ml_NSToolbar_insertItem(BriskToolbar *toolbar, id<BriskToolbarItem> item,
                             intnat position) {
  [toolbar insertItem:item index:position];
}

void ml_NSToolbar_removeItem(BriskToolbar *toolbar, id<BriskToolbarItem> item) {
  [toolbar removeItem:item];
}
