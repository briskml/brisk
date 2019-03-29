#import "BriskViewable.h"

NSView *ml_BriskView_make() {
  NSView *view = [NSView new];
  retainView(view);

  [view setWantsLayer:YES];
  view.layer.masksToBounds = NO;

  return view;
}

void ml_BriskView_insertSubview(id view, id child, intnat position) {
  if ([view conformsToProtocol:@protocol(BriskViewParent)]) {
    [view brisk_insertNode:NSViewFromBriskView(child) position:position];
  } else {
    NSView *nsview = NSViewFromBriskView(view);
    [nsview addSubview:NSViewFromBriskView(child)
            positioned:NSWindowAbove
            relativeTo:(position == 0 ? nil : nsview.subviews[position - 1])];
  }
}

void ml_BriskView_removeSubview(id parent, id child) {
  if ([parent conformsToProtocol:@protocol(BriskViewParent)]) {
    [parent brisk_deleteNode:child];
  } else {
    [NSViewFromBriskView(child) removeFromSuperview];
  }
  /* Trakcing lifetimes becomes even trickier with the introduction of
   * BriskView We should move practically all of this logic to OCaml, but
   * until then this plumbing should work. By releasing the ownership only
   * when we delete the node, we won't release child until it's removed
   */
  releaseView(child);
}

void ml_BriskView_setFrame(id view, double x, double y, double w, double h,
                           double paddingLeft, double paddingRight,
                           double paddingBottom, double paddingTop) {
  NSRect rect = NSMakeRect(x, y, w, h);
  if ([view conformsToProtocol:@protocol(BriskMeasuredView)]) {
    [view brisk_setFrame:rect
             paddingLeft:paddingLeft
            paddingRight:paddingRight
              paddingTop:paddingTop
           paddingBottom:paddingBottom];
  } else {
    [NSViewFromBriskView(view) setFrame:rect];
  }
}

void ml_BriskView_setBorderWidth(id view, double width) {
  [NSViewFromBriskView(view).layer setBorderWidth:width];
}

void ml_BriskView_setBorderColor(id view, double red, double green, double blue,
                                 double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
  [NSViewFromBriskView(view).layer setBorderColor:[color CGColor]];
}

CAMLprim value ml_BriskView_setBorderColor_bc(id view, value red_v,
                                              value green_v, value blue_v,
                                              value alpha_v) {
  CAMLparam4(red_v, green_v, blue_v, alpha_v);

  ml_BriskView_setBorderColor(NSViewFromBriskView(view), Double_val(red_v),
                              Double_val(green_v), Double_val(blue_v),
                              Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskView_setBackgroundColor(NSView *view, double red, double green,
                                     double blue, double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];
  [view.layer setBackgroundColor:[color CGColor]];
}

CAMLprim value ml_BriskView_setBackgroundColor_bc(NSView *view, value red_v,
                                                  value green_v, value blue_v,
                                                  value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);

  ml_BriskView_setBackgroundColor(view, Double_val(red_v), Double_val(blue_v),
                                  Double_val(green_v), Double_val(alpha_v));

  CAMLreturn(Val_unit);
}

void ml_BriskView_setBorderRadius(NSView *view, double radius) {
  [view.layer setCornerRadius:(CGFloat)radius];
}

CAMLprim value ml_BriskView_setBorderRadius_bc(NSView *view, value radius_v) {
  CAMLparam1(radius_v);

  ml_BriskView_setBorderRadius(view, Double_val(radius_v));

  CAMLreturn(Val_unit);
}

void ml_BriskView_setShadowOffset(NSView *view, double x, double y) {
  [view.layer setShadowOffset:NSMakeSize(x, y)];
}

CAMLprim value ml_BriskView_setShadowOffset_bc(NSView *view, value x_v,
                                               value y_v) {
  CAMLparam2(x_v, y_v);
  ml_BriskView_setShadowOffset(view, Double_val(x_v), Double_val(y_v));
  CAMLreturn(Val_unit);
}

void ml_BriskView_setShadowRadius(NSView *view, double radius) {
  [view.layer setShadowRadius:(CGFloat)radius];
}

CAMLprim value ml_BriskView_setShadowRadius_bc(NSView *view, value radius_v) {
  CAMLparam1(radius_v);
  ml_BriskView_setShadowRadius(view, Double_val(radius_v));
  CAMLreturn(Val_unit);
}

void ml_BriskView_setShadowOpacity(NSView *view, double opacity) {
  [view.layer setShadowOpacity:(CGFloat)opacity];
}

CAMLprim value ml_BriskView_setShadowOpacity_bc(NSView *view, value opacity_v) {
  CAMLparam1(opacity_v);
  ml_BriskView_setShadowOpacity(view, Double_val(opacity_v));
  CAMLreturn(Val_unit);
}

void ml_BriskView_setShadowColor(NSView *view, double red, double green,
                                 double blue, double alpha) {
  NSColor *color = [NSColor colorWithRed:red green:green blue:blue alpha:alpha];

  [view.layer setShadowColor:[color CGColor]];
}

CAMLprim value ml_BriskView_setShadowColor_bc(NSView *view, value red_v,
                                              value green_v, value blue_v,
                                              value alpha_v) {
  CAMLparam4(red_v, blue_v, green_v, alpha_v);
  ml_BriskView_setShadowColor(view, Double_val(red_v), Double_val(blue_v),
                              Double_val(green_v), Double_val(alpha_v));
  CAMLreturn(Val_unit);
}

void ml_BriskView_setMasksToBounds(NSView *view, intnat masks) {
  view.layer.masksToBounds = (BOOL)masks;
}

CAMLprim value ml_BriskView_setMasksToBounds_bc(NSView *view, value masks_v) {
  CAMLparam1(masks_v);
  ml_BriskView_setMasksToBounds(view, Int_val(masks_v));
  CAMLreturn(Val_unit);
}
