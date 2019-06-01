include
  RunLoop.ReconcilerGroup with
    type outputNode = Brisk.OutputTree.node and
    type syntheticElement = Brisk.syntheticElement and
    type renderedElement = Brisk.RenderedElement.t and
    type root = Brisk.OutputTree.node;

let setWindowHeight: float => unit;

let setWindowWidth: float => unit;

let update:
  (
    ~previousElement: syntheticElement,
    ~renderedElement: renderedElement,
    syntheticElement
  ) =>
  renderedElement;
