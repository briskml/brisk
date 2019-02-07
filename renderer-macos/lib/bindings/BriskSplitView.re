module DividerStyle = {
  type t;

  [@noalloc]
  external thick: unit => t =
    "ml_getNSSplitViewDividerStyleThick_bc"
    "ml_getNSSplitViewDividerStyleThick";

  [@noalloc]
  external thin: unit => t =
    "ml_getNSSplitViewDividerStyleThin_bc" "ml_getNSSplitViewDividerStyleThin";

  [@noalloc]
  external paneSplitter: unit => t =
    "ml_getNSSplitViewDividerStylePaneSplitter_bc"
    "ml_getNSSplitViewDividerStylePaneSplitter";
};

type dividerStyle = [ | `Thick | `Thin | `PaneSplitter];

type style = [ | `Vertical(bool) | `DividerStyle(dividerStyle)];

type t = CocoaTypes.view;

[@noalloc] external make: unit => t = "ml_BriskSplitView_make";

[@noalloc]
external setVertical: (t, [@untagged] int) => unit =
  "ml_BriskSplitView_setVertical_bc" "ml_BriskSplitView_setVertical";

[@noalloc]
external setDividerStyle: (t, DividerStyle.t) => unit =
  "ml_BriskSplitView_setDividerStyle";

let setStyle = (split, attribute: [> style]) =>
  switch (attribute) {
  | `Vertical(vertical) => setVertical(split, vertical ? 1 : 0)
  | `DividerStyle(dividerStyle) =>
    (
      switch (dividerStyle) {
      | `Thick => DividerStyle.thick()
      | `Thin => DividerStyle.thin()
      | `PaneSplitter => DividerStyle.paneSplitter()
      }
    )
    |> setDividerStyle(split)
  | _ => ()
  };
