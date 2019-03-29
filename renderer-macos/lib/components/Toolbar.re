module OutputTree = {
  type node =
    | Window(CocoaTypes.window)
    | Toolbar(BriskToolbar.t)
    | Item(BriskToolbar.Item.t);

  let markAsStale = () => ();

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    switch (parent, child) {
    /* Leaky abstraction! */
    | (Window(window), Toolbar(toolbar)) =>
      BriskWindow.setToolbar(window, toolbar);
      parent;
    | (Toolbar(toolbar), Item(item)) =>
      BriskToolbar.insertItem(toolbar, item, position);
      parent;
    | _ => raise(Invalid_argument("Cannot add toolbar as child of item"))
    };
  };

  let deleteNode = (~parent: node, ~child: node) => {
    switch (parent, child) {
    | (Toolbar(toolbar), Item(item)) =>
      BriskToolbar.removeItem(toolbar, item);
      parent;
    | _ => raise(Invalid_argument("Cannot add toolbar as child of item"))
    };
  };

  let moveNode = (~parent, ~child as _, ~from as _, ~to_ as _) => {
    /* TODO: Implement after adding position to delete node in brisk reconciler */
    parent;
  };
};

module Reconciler = Brisk_reconciler.Make(OutputTree);

let toolbar = {
  open Reconciler;
  let component = nativeComponent("toolbar");
  (~children, ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => {
            let toolbar = BriskToolbar.make();
            Toolbar(toolbar);
          },
          configureInstance: (~isFirstRender as _, node) => node,
          children: listToElement(children),
        },
      )
    );
};

let flexibleSpace = {
  open Reconciler;
  let component = nativeComponent("flexibleSpace");
  (~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        {
          make: () => Item(BriskToolbar.Item.makeFlexibleSpace()),

          configureInstance: (~isFirstRender as _, node) => node,
          children: Reconciler.empty,
        },
      )
    );
};

let item = {
  let component = Reconciler.nativeComponent("toolbar.item");
  (~children: list(Brisk.syntheticElement), ()) =>
    component(hooks => {
      let (renderedItems, memoizeRenderedItems, hooks) =
        Reconciler.Hooks.ref(Obj.magic(), hooks);
      let (previousElement, setPreviousElement, hooks) =
        Reconciler.Hooks.ref(Brisk.listToElement(children), hooks);
      (
        hooks,
        {
          make: () => {
            Item(BriskToolbar.Item.make());
          },
          configureInstance: (~isFirstRender, node) =>
            if (isFirstRender) {
              let toolbarItem =
                switch (node) {
                | Item(item) => item
                | _ => assert(false)
                };
              open Brisk;
              let view = BriskToolbar.Item.toViewable(toolbarItem);
              let renderedItems =
                RenderedElement.render(
                  {
                    layoutNode:
                      Brisk.Layout.Node.make({view, isYAxisFlipped: false}),
                    view,
                  },
                  previousElement,
                )
                |> RenderedElement.executePendingEffects;
              let _ = RenderedElement.executeHostViewUpdates(renderedItems);
              memoizeRenderedItems(renderedItems);
              node;
            } else {
              open Brisk;
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
          children: Reconciler.empty,
        },
      );
    });
};
