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

module Make = (ReconcilerGroup: ReconcilerGroup) => {
  let renderedRef = ref(None);

  let flushPendingUpdates = () =>
    switch (renderedRef^) {
    | Some(rendered) =>
      let updated =
        rendered
        |> ReconcilerGroup.flushPendingUpdates
        |> ReconcilerGroup.executePendingEffects;
      ReconcilerGroup.setDirty(false);
      renderedRef := Some(updated);
    | _ => ()
    };

  let executeHostViewUpdates = () =>
    switch (renderedRef^) {
    | Some(rendered) =>
      ignore(ReconcilerGroup.executeHostViewUpdates(rendered))
    | _ => ()
    };

  let renderAndMount =
      (root: ReconcilerGroup.root, element: ReconcilerGroup.syntheticElement) => {
    let rendered =
      ReconcilerGroup.render(root, element)
      |> ReconcilerGroup.executePendingEffects;
    renderedRef := Some(rendered);
    executeHostViewUpdates();
    /* FIXME: The event handler has to be added last because
     * executeHostViewUpdates might end up recursively calling
     * the event handler which in turn calls executeHostViewUpdates.
     * If there were some events on the native side. It's suboptimal.
     */
    UIEventCallback.addEventHandler(() => {
      flushPendingUpdates();
      executeHostViewUpdates();
    });
  };

  let rec run = () => {
    /*
     * Mediocre abstraction. That said we will take another look at this
     * when we refactor the reconciler. It should provide completely
     * new abstractions for nesting reconcilers
     */

    Lwt.wakeup_paused();
    /*
     * iter will return when an fd becomes ready for reading or writing
     * you can force LWTFakeIOEvent to start a new iteration
     */
    Lwt_engine.iter(true);
    Lwt.wakeup_paused();
    if (ReconcilerGroup.isDirty()) {
      flushPendingUpdates();
      GCD.dispatchSyncMain(executeHostViewUpdates);
    };
    run();
  };

  let spawn = () => {
    GCD.dispatchAsyncBackground(run);
  };
};
