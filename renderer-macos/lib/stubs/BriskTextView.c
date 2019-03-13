#import "BriskStylableText.h"
#import "BriskViewable.h"

@interface BriskTextView : NSTextView <BriskStylableText, BriskViewable>
@property(nonatomic, assign) CGFloat paddingLeft;
@property(nonatomic, assign) CGFloat paddingTop;
@end

@implementation BriskTextView

@synthesize brisk_paragraphStyle;

- (id)init {
  self = [super init];
  if (self) {
    self.wantsLayer = YES;
    /* This avoids automatic resizing by NSLayoutManager
     * based on NSTextContainer's calculated text size alone
     * which conflicts with our manual frame setting inside
     * brisk_setFrame.
     */
    self.verticallyResizable = NO;
    self.horizontallyResizable = NO;
    self.brisk_paragraphStyle =
        [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
    self.backgroundColor = [NSColor clearColor];
    self.textContainer.widthTracksTextView = NO;
    self.textContainer.heightTracksTextView = NO;
    /* It seems that if we don't track width and height of the view,
     * the text container inset has no result
     */
    self.textContainerInset = CGSizeZero;
    // We calculate paddings manually and override textContainerOrigin
    self.textContainer.lineFragmentPadding = 0.;

    self.paddingLeft = 0.;
    self.paddingTop = 0.;
  }
  return self;
}

- (BOOL)isEditable {
  return NO;
}

- (BOOL)isSelectable {
  return NO;
}

#pragma mark - BriskStylableText

- (void)brisk_setLineBreakMode:(NSLineBreakMode)mode {
  self.textContainer.lineBreakMode = mode;
}

- (void)brisk_beginTextStyleChanges {
  [self.textStorage beginEditing];
}

- (void)brisk_applyTextStyle {
  [self brisk_addAttribute:NSParagraphStyleAttributeName
                     value:self.brisk_paragraphStyle];
  [self.textStorage endEditing];
}

- (void)brisk_addAttribute:(NSAttributedStringKey)attribute value:(id)value {
  [self.textStorage addAttribute:attribute
                           value:value
                           range:NSMakeRange(0, self.textStorage.length)];
}

#pragma mark - BriskViewable

- (void)brisk_setFrame:(CGRect)nextFrame
           paddingLeft:(CGFloat)paddingLeft
          paddingRight:(CGFloat)paddingRight
            paddingTop:(CGFloat)paddingTop
         paddingBottom:(CGFloat)paddingBottom {
  CGFloat width = nextFrame.size.width;
  CGFloat height = nextFrame.size.height;
  self.textContainer.size = CGSizeMake(width - paddingLeft - paddingRight,
                                       height - paddingBottom - paddingTop);
  self.paddingTop = paddingTop;
  self.paddingLeft = paddingLeft;
  [self setFrame:nextFrame];
}

- (NSPoint)textContainerOrigin {
  return NSMakePoint(self.paddingLeft, self.paddingTop);
}

- (void)brisk_insertNode:(NSView __unused *)child
                position:(intnat __unused)position {
  @throw([NSException
      exceptionWithName:@"Cannot insert a node to a TextView"
                 reason:@"Nesting views inside TextView is not supported"
               userInfo:nil]);
}

@end

BriskTextView *ml_BriskTextView_make() {
  BriskTextView *txt = [BriskTextView new];
  retainView(txt);

  return txt;
}

void ml_BriskTextView_setTextContainerSize(BriskTextView *txt, double width,
                                           double height) {
  txt.textContainer.containerSize = NSMakeSize(width, height);
  [txt.layoutManager ensureLayoutForTextContainer:txt.textContainer];
}

double ml_BriskTextView_getTextWidth(BriskTextView *txt) {
  return
      [txt.layoutManager usedRectForTextContainer:txt.textContainer].size.width;
}

double ml_BriskTextView_getTextHeight(BriskTextView *txt) {
  return [txt.layoutManager usedRectForTextContainer:txt.textContainer]
      .size.height;
}

CAMLprim value ml_BriskTextView_getTextWidth_bc(BriskTextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextView_getTextWidth(txt)));
}

CAMLprim value ml_BriskTextView_getTextHeight_bc(BriskTextView *txt) {
  CAMLparam0();
  CAMLreturn(caml_copy_double(ml_BriskTextView_getTextHeight(txt)));
}

CAMLprim value ml_BriskTextView_setStringValue(BriskTextView *txt,
                                               value str_v) {
  CAMLparam1(str_v);

  NSString *str = [NSString stringWithUTF8String:String_val(str_v)];

  [txt.textStorage.mutableString setString:str];
  [txt brisk_applyTextStyle];

  CAMLreturn(Val_unit);
}

void ml_BriskTextView_setBackgroundColor(BriskTextView *txt, double red,
                                         double green, double blue,
                                         double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
  [txt setBackgroundColor:color];
}

CAMLprim value ml_BriskTextView_setBackgroundColor_bc(BriskTextView *txt,
                                                      value red_v,
                                                      value green_v,
                                                      value blue_v,
                                                      value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskTextView_setBackgroundColor(txt, Double_val(red_v),
                                      Double_val(blue_v), Double_val(green_v),
                                      Double_val(alpha_v));

  CAMLreturn(Val_unit);
}
