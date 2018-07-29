module type HostImplementation = {
  type hostView;
  let getInstance: int => option(hostView);
  let memoizeInstance: (int, hostView) => unit;

  let markAsDirty: unit => unit;

  let beginChanges: unit => unit;

  let mountChild:
    (~parent: hostView, ~child: hostView, ~position: int) => unit;
  let unmountChild: (~parent: hostView, ~child: hostView) => unit;

  let remountChild:
    (~parent: hostView, ~child: hostView, ~position: int) => unit;

  let commitChanges: unit => unit;
};

module Make:
  (Implementation: HostImplementation) =>
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
    module Key: {type t; let create: unit => t;};
    type sideEffects;
    type update('state, 'action) =
      | NoUpdate
      | Update('state);
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
      reduce: 'payload .reduce('payload, 'action),
      act: 'action => unit,
    };

    /** Type of a react element before rendering */
    type reactElement;
    type nativeElement('state, 'action) = {
      make: unit => Implementation.hostView,
      updateInstance:
        (self('state, 'action), Implementation.hostView) => unit,
      shouldReconfigureInstance:
        (~oldState: 'state, ~newState: 'state) => bool,
      children: reactElement,
    };
    type elementType('concreteElementType, 'state, 'action);
    type instance('state, 'action, 'elementType);
    type oldNewSelf('state, 'action) = {
      oldSelf: self('state, 'action),
      newSelf: self('state, 'action),
    };
    type handedOffInstance('state, 'action, 'elementType);
    type componentSpec('state, 'initialState, 'action, 'elementType) = {
      debugName: string,
      elementType: elementType('elementType, 'state, 'action),
      willReceiveProps: self('state, 'action) => 'state,
      didMount: self('state, 'action) => unit,
      didUpdate: oldNewSelf('state, 'action) => unit,
      willUnmount: self('state, 'action) => unit /* TODO: currently unused */,
      willUpdate: oldNewSelf('state, 'action) => unit,
      shouldUpdate: oldNewSelf('state, 'action) => bool,
      render: self('state, 'action) => 'elementType,
      initialState: unit => 'initialState,
      reducer: ('action, 'state) => update('state, 'action),
      printState: 'state => string /* for internal debugging */,
      handedOffInstance: handedOffInstance('state, 'action, 'elementType),
      key: Key.t,
    };
    type component('state, 'action, 'elementType) =
      componentSpec('state, 'state, 'action, 'elementType);
    type stateless = unit;
    type actionless = unit;
    let statelessComponent:
      (~useDynamicKey: bool=?, string) =>
      component(stateless, actionless, reactElement);
    let statefulComponent:
      (~useDynamicKey: bool=?, string) =>
      componentSpec('state, stateless, actionless, reactElement);
    let reducerComponent:
      (~useDynamicKey: bool=?, string) =>
      componentSpec('state, stateless, 'action, reactElement);
    let statelessNativeComponent:
      (~useDynamicKey: bool=?, string) =>
      component(stateless, actionless, nativeElement(stateless, actionless));
    let element:
      (~key: Key.t=?, component('state, 'action, 'elementType)) =>
      reactElement;
    let arrayToElement: array(reactElement) => reactElement;
    let listToElement: list(reactElement) => reactElement;
    let logString: string => unit;

    /**
     * Log of operations performed to update an instance tree.
     */
    module UpdateLog: {
      type subtreeChange;
      type entry;
      type t;
      let create: unit => t;
    };

    module RenderedElement: {
      /** Type of a react element after rendering  */
      type t;
      type topLevelUpdate;
      let listToRenderedElement: list(t) => t;

      /** Render one element by creating new instances. */
      let render: reactElement => t;

      /** Update a rendered element when a new react element is received. */
      let update:
        (
          ~previousReactElement: reactElement,
          ~renderedElement: t,
          reactElement
        ) =>
        (t, option(topLevelUpdate));

      /** Flush pending state updates (and possibly add new ones). */
      let flushPendingUpdates: t => (t, UpdateLog.t);
    };

    /**
     * Functions for mounting/unmounting an instance tree from
     * rendered element, update log, and top level update.
     */
    module HostView: {
      let mountRenderedElement:
        (Implementation.hostView, RenderedElement.t) => unit;

      let applyUpdateLog:
        (Implementation.hostView, list(UpdateLog.entry)) => unit;

      let applyTopLevelUpdate:
        (
          Implementation.hostView,
          RenderedElement.t,
          option(RenderedElement.topLevelUpdate)
        ) =>
        unit;
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
