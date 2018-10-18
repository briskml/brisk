open Brisk_core;
open Cocoa;

module Node = {
  type context = NSView.t;
  let nullContext = NSView.make();
};

module Layout = Flex.Layout.Create(Node, Flex.FloatEncoding);

let makeLayoutNode = (~layout, hostView) =>
  Layout.LayoutSupport.createNode(
    ~withChildren=[||],
    ~andStyle=layout,
    hostView,
  );

module NativeCocoa = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement = nsView;

  [@deriving (show({with_path: false}), eq)]
  type hostView = {
    view: hostElement,
    layoutNode: Layout.LayoutSupport.LayoutTypes.node,
  };

  let cssNodeInsertChild = (node, child, index) => {
    open Layout.LayoutSupport;
    open LayoutTypes;
    assert(child.parent === theNullNode);
    assert(node.measure === None);
    let capacity = Array.length(node.children);
    if (capacity == node.childrenCount) {
      /* TODO:Simply use Array.fill (no need to allocate a separate `fill` array
       * */
      let fill = Array.make(capacity + 1, theNullNode);
      Array.blit(node.children, 0, fill, 0, capacity);
      node.children = fill;
    };
    for (i in node.childrenCount downto index + 1) {
      node.children[i] = node.children[i - 1];
    };
    node.childrenCount = node.childrenCount + 1;
    node.children[index] = child;
    child.parent = node;
    Layout.LayoutSupport.markDirtyInternal(node);
  };

  let instanceMap: Hashtbl.t(int, hostView) = Hashtbl.create(1000);

  let getInstance: int => option(hostView) =
    id =>
      Hashtbl.(mem(instanceMap, id) ? Some(find(instanceMap, id)) : None);

  let memoizeInstance = (id, instance) => {
    Hashtbl.add(instanceMap, id, instance);
    NSView.memoize(id, instance.view);
  };

  let freeInstance = id => {
    Hashtbl.remove(instanceMap, id);
    NSView.free(id);
  };

  let isDirty = ref(false);
  let markAsDirty = () => isDirty := true;

  let beginChanges = () => ();

  let commitChanges = () => ();

  let mountChild = (~parent: hostView, ~child: hostView, ~position: int) => {
    cssNodeInsertChild(parent.layoutNode, child.layoutNode, position);
    NSView.addSubview(parent.view, child.view, position);
  };

  let unmountChild = (~parent as _, ~child) =>
    NSView.removeSubview(child.view);

  let remountChild = (~parent as _, ~child as _, ~position as _) => ();
};

include ReactCore.Make(NativeCocoa);

module RunLoop = {
  let rootRef = ref(None);
  let renderedRef = ref(None);
  let heightRef = ref(0.);

  let setWindowHeight = height => heightRef := height;

  let rec traverseAndRunLayout =
          (~height, node: Layout.LayoutSupport.LayoutTypes.node) => {
    let layout = node.layout;

    let nodeTop = float_of_int(layout.top);
    let nodeHeight = layout.height |> float_of_int;
    let flippedTop = height -. nodeHeight -. nodeTop;

    NSView.setFrame(
      node.context,
      layout.left |> float_of_int,
      flippedTop,
      layout.width |> float_of_int,
      nodeHeight,
    );

    node.children |> Array.iter(child => traverseAndRunLayout(~height=nodeHeight, child));
  };

  let performLayout = (~height, root: NativeCocoa.hostView) => {
    let node = root.layoutNode;
    Layout.(
      layoutNode(
        node,
        Flex.FixedEncoding.cssUndefined,
        Flex.FixedEncoding.cssUndefined,
        Ltr,
      )
    );
    traverseAndRunLayout(~height, node);
  };

  let loop = () =>
    switch (rootRef^, renderedRef^) {
    | (Some(root), Some(rendered)) =>
      if (NativeCocoa.isDirty^ === true) {
        let (nextElement, updateLog) =
          RenderedElement.flushPendingUpdates(rendered);
        HostView.applyUpdateLog(root, updateLog);
        performLayout(~height=heightRef^, root);
        NativeCocoa.isDirty := false;
        renderedRef := Some(nextElement);
      }
    | _ => ignore()
    };

  let run = (~height, root: NativeCocoa.hostView, element: reactElement) => {
    let rendered = RenderedElement.render(element);
    HostView.mountRenderedElement(root, rendered);
    performLayout(~height, root);

    setWindowHeight(height);

    rootRef := Some(root);
    renderedRef := Some(rendered);
  };
};
