module Layout =
  Brisk_core.CreateLayout(
    {
      type context = BriskView.t;
      let nullContext = BriskView.make();
    },
    Flex.FloatEncoding,
  );

module OutputTree = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement = CocoaTypes.view;

  [@deriving (show({with_path: false}), eq)]
  type node = {
    view: hostElement,
    layoutNode: Layout.Node.t,
  };

  let instanceMap: Hashtbl.t(int, node) = Hashtbl.create(1000);

  let isDirty = ref(false);

  let markAsStale = () => {
    isDirty := true;
  };

  let beginChanges = () => ();

  let commitChanges = () => ();

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    open Layout.Node;

    let parentNode = parent.layoutNode.content;
    let childNode = child.layoutNode.container;

    insertChild(parentNode, childNode, position);
    BriskView.insertSubview(parentNode.context, childNode.context, position);
    parent;
  };

  let deleteNode = (~parent, ~child) => {
    BriskView.removeSubview(child.view);
    parent;
  };

  let moveNode = (~parent, ~child as _, ~from as _, ~to_ as _) => parent;
};

include Brisk_reconciler.Make(OutputTree);

module UI = {
  let rootRef = ref(None);
  let renderedRef = ref(None);
  let heightRef = ref(0.);

  let setWindowHeight = height => {
    heightRef := height;
  };

  module Layout = {
    let rec traverseAndApply = (~height, node: Layout.Node.flexNode) => {
      let layout = node.layout;

      let nodeHeight = layout.height |> float_of_int;
      let nodeTop = nodeHeight +. float_of_int(layout.top);

      let flippedTop = height >= nodeTop ? height -. nodeTop : height;

      BriskView.setFrame(
        node.context,
        layout.left |> float_of_int,
        flippedTop,
        layout.width |> float_of_int,
        nodeHeight,
      );

      node.children
      |> Array.iter(child => traverseAndApply(~height=nodeHeight, child));
    };

    let perform = (~height, root: OutputTree.node) => {
      let node = root.layoutNode.container;
      Layout.FlexLayout.(
        layoutNode(
          node,
          Flex.FixedEncoding.cssUndefined,
          Flex.FixedEncoding.cssUndefined,
          Ltr,
        )
      );
      traverseAndApply(~height, node);
    };
  };

  let flushPendingUpdates = () =>
    switch (renderedRef^) {
    | Some(rendered) =>
      let updated =
        rendered
        |> RenderedElement.flushPendingUpdates
        |> RenderedElement.executePendingEffects;
      OutputTree.isDirty := false;
      renderedRef := Some(updated);
    | _ => ()
    };

  let executeHostViewUpdatesAndLayout = () =>
    switch (rootRef^, renderedRef^) {
    | (Some(root), Some(rendered)) =>
      RenderedElement.executeHostViewUpdates(rendered) |> ignore;
      Layout.perform(~height=heightRef^, root);
    | _ => ()
    };

  let renderAndMount =
      (~height, root: OutputTree.node, element: syntheticElement) => {
    let rendered = RenderedElement.render(root, element);
    rootRef := Some(root);
    renderedRef := Some(rendered);
    heightRef := height;
    executeHostViewUpdatesAndLayout();
  };
};

module RunLoop = {
  let rec run = () => {
    Lwt.wakeup_paused();
    /*
     * iter will return when an fd becomes ready for reading or writing
     * you can force LWTFakeIOEvent to start a new iteration
     */
    Lwt_engine.iter(true);
    Lwt.wakeup_paused();
    if (OutputTree.isDirty^) {
      UI.flushPendingUpdates();
      GCD.dispatchSyncMain(UI.executeHostViewUpdatesAndLayout);
    };
    run();
  };
  let spawn = () => {
    GCD.dispatchAsyncBackground(run);
  };
};
