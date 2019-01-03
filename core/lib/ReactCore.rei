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

    module Callback: {
      /**
       * Type for callbacks
       *
       * This type can be left abstract to prevent calling the callback directly.
       * For example, calling `update handler event` would force an immediate
       * call of `handler` with the current state, and can be prevented by defining:
       *
       *   type t 'payload;
       *
       * However, we do want to support immediate calling of a handler, as an
       * escape hatch for the existing async setState reactJS pattern
       */
      type t('payload) = 'payload => unit;

      /** Default no-op callback */
      let default: t('payload);

      /** Chain two callbacks by executing the first before the second one */
      let chain: (t('payload), t('payload)) => t('payload);
    };

    type reduce('payload, 'action) =
      ('payload => 'action) => Callback.t('payload);

    type self('state, 'action) = {
      state: 'state,
      reduce: 'payload. reduce('payload, 'action),
      act: 'action => unit,
    };

    /** Type of element returned from render */
    type syntheticElement;

    /** Type of element that renders an output node */
    type outputTreeElement('state, 'action) = {
      make: unit => OutputTree.node,
      updateInstance:
        (self('state, 'action), OutputTree.node) => OutputTree.node,
      shouldReconfigureInstance:
        (~oldState: 'state, ~newState: 'state) => bool,
      children: syntheticElement,
    };

    type elementType('concreteElementType, 'outputNodeType, 'state, 'action);

    type oldNewSelf('state, 'action) = {
      oldSelf: self('state, 'action),
      newSelf: self('state, 'action),
    };

    type update('state, 'action) =
      | NoUpdate
      | Update('state);

    type handedOffInstance('state, 'action, 'elementType, 'outputNodeType);

    type componentSpec(
      'state,
      'initialState,
      'action,
      'elementType,
      'outputNode,
    ) = {
      debugName: string,
      elementType: elementType('elementType, 'outputNode, 'state, 'action),
      willReceiveProps: self('state, 'action) => 'state,
      didMount: self('state, 'action) => unit,
      didUpdate: oldNewSelf('state, 'action) => unit,
      willUnmount: self('state, 'action) => unit /* TODO: currently unused */,
      shouldUpdate: oldNewSelf('state, 'action) => bool,
      render: self('state, 'action) => 'elementType,
      initialState: unit => 'initialState,
      reducer: ('action, 'state) => update('state, 'action),
      printState: 'state => string /* for internal debugging */,
      handedOffInstance:
        handedOffInstance('state, 'action, 'elementType, 'outputNode),
      key: Key.t,
    };
    type component('state, 'action, 'elementType, 'outputNodeType) =
      componentSpec('state, 'state, 'action, 'elementType, 'outputNodeType);

    type stateless = unit;
    type actionless = unit;
    type outputNodeGroup;
    type outputNodeContainer;

    type syntheticComponentSpec('state, 'action) =
      componentSpec(
        'state,
        stateless,
        'action,
        syntheticElement,
        outputNodeGroup,
      );

    type outputTreeComponentSpec('state, 'action) =
      componentSpec(
        'state,
        stateless,
        'action,
        outputTreeElement('state, 'action),
        outputNodeContainer,
      );

    let statelessComponent:
      (~useDynamicKey: bool=?, string) =>
      syntheticComponentSpec(stateless, actionless);
    let statefulComponent:
      (~useDynamicKey: bool=?, string) =>
      syntheticComponentSpec('state, actionless);
    let reducerComponent:
      (~useDynamicKey: bool=?, string) =>
      syntheticComponentSpec('state, 'action);
    let statelessNativeComponent:
      (~useDynamicKey: bool=?, string) =>
      outputTreeComponentSpec(stateless, actionless);
    let element:
      (
        ~key: Key.t=?,
        component('state, 'action, 'elementType, 'hostOutputNode)
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
    };

    /**
     * RemoteAction provides a way to send actions to a remote component.
     * The sender creates a fresh RemoteAction and passes it down.
     * The recepient component calls subscribe in the didMount method.
     * The caller can then send actions to the recipient components via act.
     */
    module RemoteAction: {
      type t('action);

      /** Create a new remote action, to which one component will subscribe. */
      let create: unit => t('action);

      /** Subscribe to the remote action, via the component's `act` function. */
      let subscribe: (~act: 'action => unit, t('action)) => unit;

      /** Perform an action on the subscribed component. */

      let act: (t('action), ~action: 'action) => unit;
    };
  };
