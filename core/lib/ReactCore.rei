module type OutputTree = {
  type node;

  let markAsStale: unit => unit;

  let beginChanges: unit => unit;
  let commitChanges: unit => unit;

  let insertNode: (~parent: node, ~child: node, ~position: int) => node;
  let deleteNode: (~parent: node, ~child: node) => node;
  let moveNode: (~parent: node, ~child: node, ~from: int, ~to_: int) => node;
};

module Make:
  (OutputTree: OutputTree) =>
   {
    module GlobalState: {
      let debug: ref(bool);
      let reset: unit => unit;

      /**
       * Use physical equality to recognize that an element was added to the list of children.
       * Note: this currently does not check for pending updates on components in the list.
       */
      let useTailHack: ref(bool);
    };

    module Key: {
      type t;
      let create: unit => t;
    };

    /** Type of element returned from render */
    type syntheticElement;

    /** Type of element that renders an output node */
    type outputTreeElement('slots, 'nextSlots) = {
      make: unit => OutputTree.node,
      configureInstance:
        (~isFirstRender: bool, OutputTree.node) => OutputTree.node,
      children: syntheticElement,
    };

    type elementType('concreteElementType, 'outputNodeType, 'state, 'action);

    type outputNodeGroup;
    type outputNodeContainer;

    type component('slots, 'nextSlots, 'elementType, 'outputNodeType);

    let element:
      (
        ~key: Key.t=?,
        component('slots, 'nextSlots, 'elementType, 'hostOutputNode)
      ) =>
      syntheticElement;

    let listToElement: list(syntheticElement) => syntheticElement;

    module RenderedElement: {
      /** Type of a react element after rendering  */
      type t;

      /** Render one element by creating new instances. */
      let render: (OutputTree.node, syntheticElement) => t;

      /** Update a rendered element when a new react element is received. */
      let update:
        (
          ~previousElement: syntheticElement,
          ~renderedElement: t,
          syntheticElement
        ) =>
        t;

      /** Flush pending state updates (and possibly add new ones). */
      let flushPendingUpdates: t => t;

      let executeHostViewUpdates: t => OutputTree.node;

      let executePendingEffects: t => t;
    };

    let component:
      (
        ~useDynamicKey: bool=?,
        string,
        Hooks.t('slots, 'nextSlots) => syntheticElement
      ) =>
      component('slots, 'nextSlots, syntheticElement, outputNodeGroup);

    let nativeComponent:
      (
        ~useDynamicKey: bool=?,
        string,
        Hooks.t('slots, 'nextSlots) => outputTreeElement('slots, 'nextSlots)
      ) =>
      component(
        'slots,
        'nextSlots,
        outputTreeElement('slots, 'nextSlots),
        outputNodeContainer,
      );

    module Slots = Slots;
    module Hooks = Hooks;
    module RemoteAction = RemoteAction;
  };
