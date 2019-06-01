module type ReconcilerGroup = {
  type root;

  type outputNode;

  type syntheticElement;

  type renderedElement;

  let isDirty: unit => bool;

  let setDirty: bool => unit;

  let render: (root, syntheticElement) => renderedElement;

  let flushPendingUpdates: renderedElement => renderedElement;

  let executePendingEffects: renderedElement => renderedElement;

  let executeHostViewUpdates: renderedElement => outputNode;
};

module Make:
  (ReconcilerGroup: ReconcilerGroup) =>
   {
    let renderAndMount:
      (ReconcilerGroup.root, ReconcilerGroup.syntheticElement) => unit;

    let spawn: unit => unit;
  };
