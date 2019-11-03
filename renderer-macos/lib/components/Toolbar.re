let%nativeComponent toolbar =
  (~children, ()) =>
    (hooks =>
      (
        hooks,
        {
          make: BriskToolbar.make,
          configureInstance: (~isFirstRender as _, node) => node,
          children: listToElement(children),
          insertNode: (~parent: node, ~child: node, ~position: int) => {
            BriskToolbar.insertItem(toolbar, item, position);
            parent;
          },
          deleteNode: (~parent: node, ~child: node, ~position as _) => {
            BriskToolbar.removeItem(toolbar, item);
            parent;
          },
        },
      )
    );

let%nativeComponent flexibleSpace =
  () =>
    hooks =>
      (
        {
          make: BriskToolbar.Item.makeFlexibleSpace,
          configureInstance: (~isFirstRender as _, node) => node,
          children: Reconciler.empty,
        },
        hooks,
      )
    );

let%nativeComponent item =
  (~children: list(Brisk.syntheticElement), ()) =>
    (hooks => {
      (
        {
          make: BriskToolbar.Item.make,
          configureInstance: (~isFirstRender as _, node) => node,
          children,
        },
        hooks,
      );
    });
