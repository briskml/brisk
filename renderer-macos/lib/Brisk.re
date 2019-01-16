open Brisk_core;

module NativeCocoa = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement = BriskCocoa.view;

  [@deriving (show({with_path: false}), eq)]
  type node = {
    view: hostElement,
    layoutNode: Layout.LayoutSupport.LayoutTypes.node,
  };

  let instanceMap: Hashtbl.t(int, node) = Hashtbl.create(1000);

  [@noalloc] external markAsStale: unit => unit = "ml_schedule_layout_flush";

  let beginChanges = () => ();

  let commitChanges = () => ();

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    Layout.cssNodeInsertChild(parent.layoutNode, child.layoutNode, position);
    BriskView.addSubview(parent.view, child.view);
    parent;
  };

  let deleteNode = (~parent, ~child) => {
    BriskView.removeSubview(child.view);
    parent;
  };

  let moveNode = (~parent, ~child as _, ~from as _, ~to_ as _) => parent;
};

include ReactCore.Make(NativeCocoa);

module RunLoop = {
  let rootRef = ref(None);
  let renderedRef = ref(None);
  let heightRef = ref(0.);

  let setWindowHeight = height => {
    heightRef := height;
    NativeCocoa.markAsStale();
  };

  let rec traverseAndApplyLayout =
          (~height, node: Layout.LayoutSupport.LayoutTypes.node) => {
    let layout = node.layout;

    let nodeTop = float_of_int(layout.top);
    let nodeHeight = layout.height |> float_of_int;
    let flippedTop = height -. nodeHeight -. nodeTop;

    BriskView.setFrame(
      node.context,
      layout.left |> float_of_int,
      flippedTop,
      layout.width |> float_of_int,
      nodeHeight,
    );

    node.children
    |> Array.iter(child => traverseAndApplyLayout(~height=nodeHeight, child));
  };

  let performLayout = (~height, root: NativeCocoa.node) => {
    let node = root.layoutNode;
    Layout.(
      layoutNode(
        node,
        Flex.FixedEncoding.cssUndefined,
        Flex.FixedEncoding.cssUndefined,
        Ltr,
      )
    );
    traverseAndApplyLayout(~height, node);
  };

  let flushAndLayout = () =>
    switch (rootRef^, renderedRef^) {
    | (Some(root), Some(rendered)) =>
      let nextElement = RenderedElement.flushPendingUpdates(rendered);
      RenderedElement.executeHostViewUpdates(rendered) |> ignore;
      performLayout(~height=heightRef^, root);
      renderedRef := Some(nextElement);
    | _ => ignore()
    };

  let renderAndMount =
      (~height, root: NativeCocoa.node, element: syntheticElement) => {
    let rendered = RenderedElement.render(root, element);
    RenderedElement.executeHostViewUpdates(rendered) |> ignore;
    setWindowHeight(height);
    rootRef := Some(root);
    renderedRef := Some(rendered);
  };
};
