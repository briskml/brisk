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

  let deleteNode = (~parent: node, ~child: node, ~position as _) => {
    open Layout.Node;

    let parentNode = parent.layoutNode.content;
    let childNode = child.layoutNode.container;

    removeChild(parentNode, childNode);
    BriskView.removeSubview(parentNode.context.view, childNode.context.view);
    parent;
  };

  let moveNode = (~parent, ~child as _, ~from as _, ~to_ as _) => parent;
};

include Brisk_reconciler.Make(OutputTree);
