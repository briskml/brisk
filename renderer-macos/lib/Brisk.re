type hostContext = {
  view: BriskView.t,
  /***
   * Y coordinate could start from the bottom in AppKit.
   * By setting `isYAxisFlipped` to `false`, we can manually flip the coordinate base
   * to treat 0 as the top (we simply add the window content view height to account for flipping).
   *
   * This is not renderer-wide because some elements like NSScrollView, NSTextView, or NSButton
   * use flipped Y axis, i.e their 0 start from the top. ¯\_(ツ)_/¯
   */
  isYAxisFlipped: bool,
};

module Layout =
  Brisk_core.CreateLayout(
    {
      type context = hostContext;
      let nullContext = {view: BriskView.make(), isYAxisFlipped: false};
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

  let isDirty = ref(false);

  let markAsStale = () => {
    isDirty := true;
  };

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    open Layout.Node;

    let parentNode = parent.layoutNode.content;
    let childNode = child.layoutNode.container;

    insertChild(parentNode, childNode, position);
    BriskView.insertSubview(
      parentNode.context.view,
      childNode.context.view,
      position,
    );
    parent;
  };

  let deleteNode = (~parent: node, ~child: node) => {
    open Layout.Node;

    let parentNode = parent.layoutNode.content;
    let childNode = child.layoutNode.container;

    removeChild(parentNode, childNode);
    BriskView.removeSubview(childNode.context.view);
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
    let get0IfUndefined = x => {
      let floatValue = x;
      Layout.isUndefined(floatValue) ? 0. : floatValue;
    }

    let rec traverseAndApply = (~flip, ~height, node: Layout.Node.flexNode) => {
      let layout = node.layout;

      let nodeHeight = layout.height;
      let nodeTop = nodeHeight +. layout.top;

      let top =
        flip
          ? height >= nodeTop ? height -. nodeTop : height
          : layout.top;

      let style = node.style;

      BriskView.setFrame(
        node.context.view,
        layout.left,
        top,
        layout.width,
        nodeHeight,
        style.paddingLeft |> get0IfUndefined,
        style.paddingRight |> get0IfUndefined,
        style.paddingBottom |> get0IfUndefined,
        style.paddingTop |> get0IfUndefined,
      );

      let flip = !node.context.isYAxisFlipped;
      let height = nodeHeight;

      node.children
      |> Array.iter(child => traverseAndApply(~flip, ~height, child));
    };

    let perform = (~height, root: OutputTree.node) => {
      let node = root.layoutNode.container;
      let flip = node.context.isYAxisFlipped;


      Layout.FlexLayout.(
        layoutNode(
          node,
          Layout.cssUndefined,
          Layout.cssUndefined,
          Ltr,
        )
      );
      traverseAndApply(~flip, ~height, node);
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
    let rendered =
      RenderedElement.render(root, element)
      |> RenderedElement.executePendingEffects;
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
