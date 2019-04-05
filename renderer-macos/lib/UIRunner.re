open Brisk;

module Layout = {
  let get0IfUndefined = x => {
    let floatValue = x;
    Layout.isUndefined(floatValue) ? 0. : floatValue;
  };

  let rec traverseAndApply = (~flip, ~height, node: Layout.Node.flexNode) => {
    let layout = node.layout;

    let nodeHeight = layout.height;
    let nodeTop = nodeHeight +. layout.top;

    let top =
      flip ? height >= nodeTop ? height -. nodeTop : height : layout.top;

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

  let perform = (~height, ~width, root: OutputTree.node) => {
    let node = root.layoutNode.container;
    let flip = node.context.isYAxisFlipped;
    if (width != node.style.width || node.style.height != height) {
      node.style.width = width;
      node.style.height = height;
      node.isDirty = true;
    };

    Layout.FlexLayout.(
      layoutNode(node, Layout.cssUndefined, Layout.cssUndefined, Ltr)
    );
    traverseAndApply(~flip, ~height, node);
  };
};

type root = OutputTree.node;

type outputNode = OutputTree.node;

type syntheticElement = Brisk.syntheticElement;

type renderedElement = Brisk.RenderedElement.t;

let isDirty = () => Brisk.OutputTree.isDirty^;

let setDirty = bool => Brisk.OutputTree.isDirty := bool;

let render = RenderedElement.render;

let executePendingEffects = RenderedElement.executePendingEffects;

let flushPendingUpdates = RenderedElement.flushPendingUpdates;

let heightRef = ref(0.);

let widthRef = ref(0.);

let executeHostViewUpdates = rendered => {
  let outputNode = RenderedElement.executeHostViewUpdates(rendered);
  Layout.perform(~height=heightRef^, ~width=widthRef^, outputNode);
  outputNode;
};

let setWindowHeight = height =>
  if (height != heightRef^) {
    setDirty(true);
    heightRef := height;
  };

let setWindowWidth = width =>
  if (width != widthRef^) {
    setDirty(true);
    widthRef := width;
  };

let update = RenderedElement.update;
