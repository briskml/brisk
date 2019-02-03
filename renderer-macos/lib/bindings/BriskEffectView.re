
module Material = {
  type t;
  external titlebar: unit => t =
    "ml_getNSVisualEffectMaterialTitlebar_bc"
    "ml_getNSVisualEffectMaterialTitlebar";
  let titlebar = titlebar();
  external selection: unit => t =
    "ml_getNSVisualEffectMaterialSelection_bc"
    "ml_getNSVisualEffectMaterialSelection";
  let selection = selection();
  external menu: unit => t =
    "ml_getNSVisualEffectMaterialMenu_bc" "ml_getNSVisualEffectMaterialMenu";
  let menu = menu();
  external popover: unit => t =
    "ml_getNSVisualEffectMaterialPopover_bc"
    "ml_getNSVisualEffectMaterialPopover";
  let popover = popover();
  external sidebar: unit => t =
    "ml_getNSVisualEffectMaterialSidebar_bc"
    "ml_getNSVisualEffectMaterialSidebar";
  let sidebar = sidebar();
  /*
  external headerView: unit => t =
    "ml_getNSVisualEffectMaterialHeaderView_bc"
    "ml_getNSVisualEffectMaterialHeaderView";
  let headerView = headerView();
  external sheet: unit => t =
    "ml_getNSVisualEffectMaterialSheet_bc" "ml_getNSVisualEffectMaterialSheet";
  let sheet = sheet();
  external windowBackground: unit => t =
    "ml_getNSVisualEffectMaterialWindowBackground_bc"
    "ml_getNSVisualEffectMaterialWindowBackground";
  let windowBackground = windowBackground();
  external hUDWindow: unit => t =
    "ml_getNSVisualEffectMaterialHUDWindow_bc"
    "ml_getNSVisualEffectMaterialHUDWindow";
  let hUDWindow = hUDWindow();
  external fullScreenUI: unit => t =
    "ml_getNSVisualEffectMaterialFullScreenUI_bc"
    "ml_getNSVisualEffectMaterialFullScreenUI";
  let fullScreenUI = fullScreenUI();
  external toolTip: unit => t =
    "ml_getNSVisualEffectMaterialToolTip_bc"
    "ml_getNSVisualEffectMaterialToolTip";
  let toolTip = toolTip();
  external contentBackground: unit => t =
    "ml_getNSVisualEffectMaterialContentBackground_bc"
    "ml_getNSVisualEffectMaterialContentBackground";
  let contentBackground = contentBackground();
  external underWindowBackground: unit => t =
    "ml_getNSVisualEffectMaterialUnderWindowBackground_bc"
    "ml_getNSVisualEffectMaterialUnderWindowBackground";
  let underWindowBackground = underWindowBackground();
  external pageBackground: unit => t =
    "ml_getNSVisualEffectMaterialPageBackground_bc"
    "ml_getNSVisualEffectMaterialPageBackground";
  let pageBackground = pageBackground();
  external appeareanceBased: unit => t =
    "ml_getNSVisualEffectMaterialAppeareanceBased_bc"
    "ml_getNSVisualEffectMaterialAppeareanceBased";
  let appeareanceBased = appeareanceBased();
  */
};

module BlendingMode = {
  type t;
  external behindWindow: unit => t =
    "ml_getNSVisualEffectBlendingModeBehindWindow_bc"
    "ml_getNSVisualEffectBlendingModeBehindWindow";
  let behindWindow = behindWindow();
  external withinWindow: unit => t =
    "ml_getNSVisualEffectBlendingModeWithinWindow_bc"
    "ml_getNSVisualEffectBlendingModeWithinWindow";
  let wihthinWindow = withinWindow();
};

type backgroundStyle =
  | Raised
  | Lowered;

type visualEffectState =
  | FollowsWindowActiveState
  | Active
  | Inactive;

type t = CocoaTypes.view;
[@noalloc] external make: unit => t = "ml_NSVisualEffectView_make";
[@noalloc] external setMaterial: (CocoaTypes.view, Material.t) => unit = "ml_NSVisualEffectView_setMaterial";
[@noalloc] external setBlendingMode: (CocoaTypes.view, BlendingMode.t) => unit = "ml_NSVisualEffectView_setBlendingMode";
