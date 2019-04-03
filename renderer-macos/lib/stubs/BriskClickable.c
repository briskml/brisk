#import "BriskCocoa.h"
#import "OCamlClosureEventTarget.h"

@interface BriskClickable : NSView

@property(nonatomic, strong) NSClickGestureRecognizer *clickHandler;
@property(nonatomic, strong) NSTrackingArea *trackingArea;
@property(nonatomic, strong) BriskOCamlClosureEventTarget *clickTarget;
@property(nonatomic, strong) BriskOCamlClosureEventTarget *hoverTarget;
@end

@implementation BriskClickable {
  value onClick;
  value onHover;
}

- (id)init {
  self = [super init];
  if (self) {
    self.wantsLayer = true;
    self.clickHandler = [[NSClickGestureRecognizer alloc] init];
    self.clickHandler.numberOfClicksRequired = 1;
    self.clickTarget = [BriskOCamlClosureEventTarget new];
    self.clickHandler.target = self.clickTarget;
    self.clickHandler.action = @selector(performCallback0);
    self.hoverTarget = [BriskOCamlClosureEventTarget new];
    [self addGestureRecognizer:self.clickHandler];
  }
  return self;
}

// This forces our clickable view to act like button,
// accepting the click immediately even if the window is not focused.
// Otherwise, we'd have to click twice:
//   1st - to capture window screen
//   2nd - to click the view.
- (BOOL)acceptsFirstMouse:(NSEvent *)__unused ev {
  return YES;
}

#pragma mark - Handling hovering

- (void)mouseEntered:(NSEvent __unused *)theEvent {
  [self.hoverTarget performCallback1:Val_bool(1)];
}

- (void)mouseExited:(NSEvent __unused *)theEvent {
  [self.hoverTarget performCallback1:Val_bool(0)];
}

- (void)updateTrackingAreas {
  [super updateTrackingAreas];
  if (self.trackingArea != nil) {
    [self removeTrackingArea:self.trackingArea];
  }

  self.trackingArea = [[NSTrackingArea alloc]
      initWithRect:[self bounds]
           options:NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways |
                   NSTrackingCursorUpdate
             owner:self
          userInfo:nil];
  [self addTrackingArea:self.trackingArea];
}

- (void)cursorUpdate:(NSEvent __unused *)event {
  [[NSCursor pointingHandCursor] set];
}

@end

BriskClickable *ml_BriskClickable_make() {
  BriskClickable *btn = [BriskClickable new];
  retainView(btn);

  return btn;
}

CAMLprim value ml_BriskClickable_setOnClick(BriskClickable *btn,
                                            value callbackOption) {
  CAMLparam1(callbackOption);
  value callbackOpt = callbackOption;

  [btn.clickTarget setCallback:callbackOpt];

  CAMLreturn(Val_unit);
}

CAMLprim value ml_BriskClickable_setOnHover(BriskClickable *btn,
                                            value callbackOption) {
  CAMLparam1(callbackOption);
  value callbackOpt = callbackOption;

  [btn.hoverTarget setCallback:callbackOpt];

  CAMLreturn(Val_unit);
}
