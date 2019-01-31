#import "BriskViewable.h"

@interface BriskScrollView : NSScrollView <BriskViewable>

@property(retain) NSView *contents;

@end

@implementation BriskScrollView

- (id)init {
  self = [super init];

  if (self) {
    [self setBorderType:NSNoBorder];
    [self setHasVerticalScroller:YES];
    [self setHasHorizontalScroller:NO];

    self.contents = [NSView new];
    [self setDocumentView:self.contents];
  }

  return self;
}

- (void)addChild:(NSView *)child {
  [self.contents addSubview:child];
  [self setDocumentView:self.contents];
}

- (void)setFrameRect:(NSRect)rect {
  NSRect contentRect = NSMakeRect(0, 0, rect.size.width, 2000);

  [self setFrame:rect];
  [self.contents setFrame:contentRect];
}

@end

BriskScrollView *ml_BriskScrollView_make() {
  BriskScrollView *scroll = [BriskScrollView new];
  retainView(scroll);

  return scroll;
}

void ml_BriskScrollView_setContentFrame(BriskScrollView *scroll, double width,
                                        double height) {
  NSRect rect = NSMakeRect(0, 0, width, height);
  [scroll.contents setFrame:rect];
}

CAMLprim value ml_BriskScrollView_setContentFrame_bc(BriskScrollView *scroll,
                                                     value w_v, value h_v) {
  CAMLparam2(w_v, h_v);

  ml_BriskScrollView_setContentFrame(scroll, Double_val(w_v), Double_val(h_v));

  CAMLreturn(Val_unit);
}
