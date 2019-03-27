module Item = {
  type t;

  external make: unit => t =
    "ml_NSToolbarItem_make_bc" "ml_NSToolbarItem_make";
  /* TODO: introduce a type symmetric to the viewable protocol on the
     OCaml side
   */
  external toViewable: t => CocoaTypes.view = "%identity";
  external makeFlexibleSpace: unit => t =
    "ml_NSToolbarItem_makeFlexibleSpace_bc"
    "ml_NSToolbarItem_makeFlexibleSpace";
};

type t;

external make: unit => t = "ml_NSToolbar_make_bc" "ml_NSToolbar_make";
external insertItem: (t, Item.t, [@untagged] int) => unit =
  "ml_NSToolbar_insertItem_bc" "ml_NSToolbar_insertItem";
external removeItem: (t, Item.t) => unit =
  "ml_NSToolbar_removeItem_bc" "ml_NSToolbar_removeItem";
