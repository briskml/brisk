#import "BriskCocoa.h"
#import "Availability.h"

#define ACCESSOR(VALUE) \
intnat ml_get##VALUE() {\
  return VALUE;\
} \
\
value ml_get##VALUE##_bc() {\
  CAMLparam0(); \
  CAMLreturn(VALUE);\
}

#define VISUAL_EFFECT(VALUE) ACCESSOR(NSVisualEffectMaterial##VALUE)

#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_10
VISUAL_EFFECT(Titlebar)
VISUAL_EFFECT(Selection)
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_11
VISUAL_EFFECT(Menu)
VISUAL_EFFECT(Popover)
VISUAL_EFFECT(Sidebar)
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_14
VISUAL_EFFECT(HeaderView)
VISUAL_EFFECT(Sheet)
VISUAL_EFFECT(WindowBackground)
VISUAL_EFFECT(HUDWindow)
VISUAL_EFFECT(FullScreenUI)
VISUAL_EFFECT(ToolTip)
VISUAL_EFFECT(ContentBackground)
VISUAL_EFFECT(UnderWindowBackground)
VISUAL_EFFECT(PageBackground)
VISUAL_EFFECT(AppeareanceBased)
#endif

#define BLENDING_MODE(VALUE) ACCESSOR(NSVisualEffectBlendingMode##VALUE)
BLENDING_MODE(BehindWindow)
BLENDING_MODE(WithinWindow)

#define BACKGROUND_STYLE(VALUE) ACCESSOR(NSBackgroundStyle##VALUE)
BACKGROUND_STYLE(Raised)
BACKGROUND_STYLE(Lowered)

#define EFFECT_STATE(VALUE) ACCESSOR(NSVisualEffectState##VALUE)
EFFECT_STATE(FollowsWindowActiveState)
EFFECT_STATE(Active)
EFFECT_STATE(Inactive)

NSVisualEffectView *ml_NSVisualEffectView_make() {
  NSVisualEffectView *view = [NSVisualEffectView new];
  retainView(view);
  return view;
}

void ml_NSVisualEffectView_setMaterial(NSVisualEffectView *view, NSVisualEffectMaterial material) {
    view.material = material;
}

void ml_NSVisualEffectView_setBlendingMode(NSVisualEffectView *view, NSVisualEffectBlendingMode blendingMode) {
    view.blendingMode = blendingMode;
}
