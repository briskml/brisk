type node =
  | SegmentedControl(CocoaTypes.view)
  | Item(BriskSegmentedControl.Item.t);

let markAsStale = () => ();

let insertNode = (~parent: node, ~child: node, ~position: int) => {
  switch (parent, child) {
  | (SegmentedControl(parent), Item(child)) =>
    BriskSegmentedControl.insertItem(parent, child, position)
  | _ => assert(false)
  };
  parent;
};

let deleteNode = (~parent, ~child, ~position as _) => {
  switch (parent, child) {
  | (SegmentedControl(parent), Item(child)) =>
    BriskSegmentedControl.deleteItem(parent, child)
  | _ => assert(false)
  };
  parent;
};

let moveNode = (~parent as _, ~child as _, ~from as _, ~to_ as _) => {
  assert
    (false);
    /* TODO: Implement after adding position to delete node in brisk reconciler */
};

let%nativeComponent item =
  (~isSelected: bool = false, ~children as text: string, ~onClick, ()) => 
    (hooks =>
      (
        {
          make: () => {
            Item(BriskSegmentedControl.Item.make());
          },
          configureInstance: (~isFirstRender as _, node) => {
            switch (node) {
            | Item(node) =>
              BriskSegmentedControl.Item.setText(node, text);
              BriskSegmentedControl.Item.setSelected(
                node,
                isSelected ? 1 : 0,
              );
              BriskSegmentedControl.Item.setOnClick(node, onClick);
            | _ => assert(false)
            };
            node;
          },
          children: empty,
        },
        hooks,
      )
    );

let%nativeComponent segmentedControl =
  (~children: list(Reconciler.syntheticElement), ()) =>
    (hooks => {
      let (renderedItems, memoizeRenderedItems, hooks) =
        Reconciler.Hooks.ref(Obj.magic(), hooks);
      let (previousElement, setPreviousElement, hooks) =
        Reconciler.Hooks.ref(Reconciler.listToElement(children), hooks);
      (
        {
          make: () => {
            open Brisk;
            let view = BriskSegmentedControl.make();
            {
              view,
              layoutNode:
                Layout.Node.make(~style=[], {view, isYAxisFlipped: false}),
            };
          },
          configureInstance: (~isFirstRender, node) =>
            if (isFirstRender) {
              open Reconciler;
              let renderedItems =
                RenderedElement.render(
                  SegmentedControl(node.view),
                  listToElement(children),
                )
                |> RenderedElement.executePendingEffects;
              let _ = RenderedElement.executeHostViewUpdates(renderedItems);
              memoizeRenderedItems(renderedItems);
              node;
            } else {
              open Reconciler;
              let nextElement = listToElement(children);
              let renderedItems =
                RenderedElement.update(
                  ~previousElement,
                  ~renderedElement=renderedItems,
                  nextElement,
                )
                |> RenderedElement.executePendingEffects;
              let _ = RenderedElement.executeHostViewUpdates(renderedItems);
              setPreviousElement(nextElement);
              memoizeRenderedItems(renderedItems);
              node;
            },
          children: Brisk.empty,
        },
        hooks,
      );
    });
