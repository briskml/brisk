module Item = {
  type t;
  external make: unit => t =
    "ml_NSSegmentedControl_Item_make_bc" "ml_NSSegmentedControl_Item_make";
  external setText: (t, string) => unit =
    "ml_NSSegmentedControl_Item_setText_bc"
    "ml_NSSegmentedControl_Item_setText";

  /* 1 if selected */
  external setSelected: (t, [@untagged] int) => unit =
    "ml_NSSegmentedControl_Item_setSelected_bc"
    "ml_NSSegmentedControl_Item_setSelected";

  external setOnClick: (t, unit => unit) => unit =
    "ml_NSSegmentedControl_Item_setOnClick_bc"
    "ml_NSSegmentedControl_Item_setOnClick";
  let setOnClick = (item, handler) =>
    setOnClick(item, UIEventCallback.make(handler));
};

type t = CocoaTypes.view;

external make: unit => t =
  "ml_NSSegmentedControl_make_bc" "ml_NSSegmentedControl_make";

external insertItem: (t, Item.t, [@untagged] int) => unit =
  "ml_NSSegmentedControl_insertItem_bc" "ml_NSSegmentedControl_insertItem";

external deleteItem: (t, Item.t) => unit =
  "ml_NSSegmentedControl_deleteItem_bc" "ml_NSSegmentedControl_deleteItem";
