module Material = {
  type t;

  external titlebar: unit => t =
    "ml_getNSVisualEffectMaterialTitlebar_bc"
    "ml_getNSVisualEffectMaterialTitlebar";

  external selection: unit => t =
    "ml_getNSVisualEffectMaterialSelection_bc"
    "ml_getNSVisualEffectMaterialSelection";

  external menu: unit => t =
    "ml_getNSVisualEffectMaterialMenu_bc" "ml_getNSVisualEffectMaterialMenu";

  external popover: unit => t =
    "ml_getNSVisualEffectMaterialPopover_bc"
    "ml_getNSVisualEffectMaterialPopover";

  external sidebar: unit => t =
    "ml_getNSVisualEffectMaterialSidebar_bc"
    "ml_getNSVisualEffectMaterialSidebar";
};

module BlendingMode = {
  type t;

  external behindWindow: unit => t =
    "ml_getNSVisualEffectBlendingModeBehindWindow_bc"
    "ml_getNSVisualEffectBlendingModeBehindWindow";

  external withinWindow: unit => t =
    "ml_getNSVisualEffectBlendingModeWithinWindow_bc"
    "ml_getNSVisualEffectBlendingModeWithinWindow";
};

module EffectState = {
  type t;

  external followsWindowActiveState: unit => t =
    "ml_getNSVisualEffectStateFollowsWindowActiveState_bc"
    "ml_getNSVisualEffectStateFollowsWindowActiveState";

  external active: unit => t =
    "ml_getNSVisualEffectStateActive_bc" "ml_getNSVisualEffectStateActive";

  external inactive: unit => t =
    "ml_getNSVisualEffectStateInactive_bc" "ml_getNSVisualEffectStateInactive";
};

type material = [ | `Titlebar | `Selection | `Menu | `Popover | `Sidebar];
type blendingMode = [ | `BehindWindow | `WithinWindow];
type effectState = [ | `FollowsWindowActiveState | `Active | `Inactive];

type style = [
  | `Material(material)
  | `BlendingMode(blendingMode)
  | `Emphasized(bool)
  | `EffectState(effectState)
];

type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_NSVisualEffectView_make";

[@noalloc]
external setMaterial: (CocoaTypes.view, Material.t) => unit =
  "ml_NSVisualEffectView_setMaterial";
[@noalloc]
external setBlendingMode: (CocoaTypes.view, BlendingMode.t) => unit =
  "ml_NSVisualEffectView_setBlendingMode";
[@noalloc]
external setEmphasized: (CocoaTypes.view, [@untagged] int) => unit =
  "ml_NSVisualEffectView_setEmphasized_bc"
  "ml_NSVisualEffectView_setEmphasized";
[@noalloc]
external setEffectState: (CocoaTypes.view, EffectState.t) => unit =
  "ml_NSVisualEffectView_setEffectState";

let setStyle = (view, attribute: [> style]) =>
  switch (attribute) {
  | `Material(material) =>
    (
      switch (material) {
      | `Titlebar => Material.titlebar()
      | `Selection => Material.selection()
      | `Menu => Material.menu()
      | `Popover => Material.popover()
      | `Sidebar => Material.sidebar()
      }
    )
    |> setMaterial(view)
  | `BlendingMode(blendingMode) =>
    (
      switch (blendingMode) {
      | `BehindWindow => BlendingMode.behindWindow()
      | `WithinWindow => BlendingMode.withinWindow()
      }
    )
    |> setBlendingMode(view)
  | `Emphasized(emphasized) => setEmphasized(view, emphasized ? 1 : 0)
  | `EffectState(effectState) =>
    (
      switch (effectState) {
      | `FollowsWindowActiveState => EffectState.followsWindowActiveState()
      | `Active => EffectState.active()
      | `Inactive => EffectState.inactive()
      }
    )
    |> setEffectState(view)
  | _ => ()
  };
