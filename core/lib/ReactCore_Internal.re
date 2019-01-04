/** Tail-recursive functions on lists */
module ListTR = {
  let useTailRecursion = l =>
    switch (l) {
    | [_, _, _, _, _, _, _, _, _, _, ..._] => true
    | _ => false
    };
  let concat = list => {
    let rec aux = (acc, l) =>
      switch (l) {
      | [] => List.rev(acc)
      | [x, ...rest] => aux(List.rev_append(x, acc), rest)
      };
    useTailRecursion(list) ? aux([], list) : List.concat(list);
  };
  let map = (f, list) =>
    useTailRecursion(list) ?
      List.rev_map(f, List.rev(list)) : List.map(f, list);
  let map3 = (f, list1, list2, list3) => {
    let rec aux = acc =>
      fun
      | ([], [], []) => acc
      | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) =>
        aux([f((x1, x2, x3)), ...acc], (nextList1, nextList2, nextList3))
      | _ => assert(false);
    aux([], (List.rev(list1), List.rev(list2), List.rev(list3)));
  };
  let fold3 = (f, list1, list2, list3, initialValue) => {
    let rec aux = acc =>
      fun
      | ([], [], []) => acc
      | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) => {
          let nextRes = f(acc, x1, x2, x3);
          aux(nextRes, (nextList1, nextList2, nextList3));
        }
      | _ => assert(false);
    aux(initialValue, (list1, list2, list3));
  };
};

module type OutputTree = {
  type node;

  let markAsStale: unit => unit;

  let beginChanges: unit => unit;
  let commitChanges: unit => unit;

  let insertNode: (~parent: node, ~child: node, ~position: int) => node;
  let deleteNode: (~parent: node, ~child: node) => node;
  let moveNode: (~parent: node, ~child: node, ~from: int, ~to_: int) => node;
};

module Make = (OutputTree: OutputTree) => {
  module GlobalState = {
    let debug = ref(true);
    let componentKeyCounter = ref(0);
    let instanceIdCounter = ref(0);
    let reset = () => {
      debug := true;
      componentKeyCounter := 0;
      /* FIXME: To be done afterwards because we'll need to  adjust tests*/
      instanceIdCounter := 1;
    };

    /**
     * Use physical equality to recognize that an element was added to the list
     * of children.
     * Note: this currently does not check for pending updates on components in
     * the list.
     */
    let useTailHack = ref(false);
  };

  module Key = {
    type t = int;

    let equal = (==);
    let none = (-1);
    let dynamicKeyMagicNumber = 0;
    let create = () => {
      incr(GlobalState.componentKeyCounter);
      GlobalState.componentKeyCounter^;
    };
  };

  type sideEffects = unit => unit;
  type stateless = unit;
  type actionless = unit;

  module Callback = {
    type t('payload) = 'payload => unit;

    let default = _event => ();
    let chain = (handlerOne, handlerTwo, payload) => {
      handlerOne(payload);
      handlerTwo(payload);
    };
  };

  type internalOutputNode =
    | Node(OutputTree.node)
    | UpdatedNode(OutputTree.node, OutputTree.node);
  type outputNodeContainer = Lazy.t(internalOutputNode);
  type outputNodeGroup = list(outputNodeContainer);

  type reduce('payload, 'action) =
    ('payload => 'action) => Callback.t('payload);

  type update('state, 'action) =
    | NoUpdate
    | Update('state)
  and self('state, 'action) = {
    state: 'state,
    reduce: 'payload. reduce('payload, 'action),
    act: 'action => unit,
  }
  /**
   * Elements are what JSX blocks become. They represent the *potential* for a
   * component instance and state to be created / updated. They are not yet
   * instances.
   */
  and element =
    | Element(component('state, 'action, 'elementType, 'outputNode)): element
  and syntheticElement =
    | Flat(element)
    | Nested(list(syntheticElement))
  and outputTreeElement('state, 'action) = {
    make: unit => OutputTree.node,
    updateInstance:
      (self('state, 'action), OutputTree.node) => OutputTree.node,
    shouldReconfigureInstance: (~oldState: 'state, ~newState: 'state) => bool,
    children: syntheticElement,
  }
  and elementType('renderedType, 'outputNode, 'state, 'action) =
    | Host: elementType(
              outputTreeElement('state, 'action),
              outputNodeContainer,
              'state,
              'action,
            )
    | React: elementType(syntheticElement, outputNodeGroup, 'state, 'action)
  and instanceForest =
    | IFlat(opaqueInstance)
    | INested(list(instanceForest), int /*subtree size*/)
  and oldNewSelf('state, 'action) = {
    oldSelf: self('state, 'action),
    newSelf: self('state, 'action),
  }
  /**
   * This type is used to avoid Obj.magic in update.
   * We create one handedOffInstance ref per component type.
   * The componentSpec is usually recreated on every render.
   * The ref is created in functions like `statelessComponent`. Then a user
   * usually spreads this ref like so:
   * let make = ... => {...component, ...};
   * Therefore all components coming from a given `make` function usually
   * share the same ref. Then, in the update function it can be determined
   *
   * When we pass the instance to one component and we can observe the mutation
   * on the other component, it means that we've got two components of the same type.
   *
   * Additionally, we pass two instances here.
   * - The first instance in the tuple contains the original instance that
   *   the update has been started with
   * - The second instance is the same instance but after executing pending
   *   state updates
   */
  and handedOffInstance('state, 'action, 'element, 'outputNode) =
    ref(option(instance('state, 'action, 'element, 'outputNode)))
  and componentSpec('state, 'initialState, 'action, 'element, 'outputNode) = {
    debugName: string,
    elementType: elementType('element, 'outputNode, 'state, 'action),
    willReceiveProps: self('state, 'action) => 'state,
    didMount: self('state, 'action) => unit,
    didUpdate: oldNewSelf('state, 'action) => unit,
    willUnmount: self('state, 'action) => unit /* TODO: currently unused */,
    shouldUpdate: oldNewSelf('state, 'action) => bool,
    render: self('state, 'action) => 'element,
    initialState: unit => 'initialState,
    reducer: ('action, 'state) => update('state, 'action),
    printState: 'state => string /* for internal debugging */,
    handedOffInstance:
      handedOffInstance('state, 'action, 'element, 'outputNode),
    key: int,
  }
  and component('state, 'action, 'elementType, 'outputNode) =
    componentSpec('state, 'state, 'action, 'elementType, 'outputNode)
  /**
   * Elements turn into instances at the right time. These instances record the
   * most recent state among other things.
   */
  and instance('state, 'action, 'elementType, 'outputNode) = {
    component: component('state, 'action, 'elementType, 'outputNode),
    /**
     * FIXME: This may not be a good idea to hold onto for sake of memory, but it makes
     * it convenient to implement shouldComponentUpdate.
     */
    element,
    iState: 'state,
    /**
     * Most recent subtree of instances.
     */
    instanceSubTree: instanceForest,
    subElements: 'elementType,
    /**
     * List of state updates pending for this instance.
     * Stored in reverse order.
     */
    pendingStateUpdates: ref(list('state => update('state, 'action))),
    /**
     * Unique instance id.
     */
    id: int,
    /**
     * A reference to the output node(s) of a given element.
     */
    hostInstance: 'outputNode,
  }
  /**
   * Opaque wrapper around `instance`, which allows many instances to be
   * commingled in a single data structure. The GADT hides the type parameters.
   * The result of "rendering" an Element is a tree of produced instances.
   * This tree is then updated to produce a new *version* of the
   * instance tree based on the old - the old one is not mutated.
   */
  and opaqueInstance =
    | Instance(instance('state, 'action, 'elementType, 'outpuNode))
      : opaqueInstance;

  type renderedElement = {
    nearestHostOutputNode: outputNodeContainer,
    instanceForest,
  };

  module InstanceForest = {
    let getSubtreeSize =
      fun
      | INested(_, x) => x
      | IFlat(Instance({hostInstance, component: {elementType}})) =>
        switch (elementType) {
        | React => List.length(hostInstance)
        | Host => 1
        };

    let rec fold =
            (f, renderedElement, nearestHostOutputNode: outputNodeContainer) =>
      switch (renderedElement) {
      | IFlat(e) =>
        let (nearestHostOutputNode, nextE) = f(e, nearestHostOutputNode);
        let unchanged = e === nextE;

        (nearestHostOutputNode, unchanged ? renderedElement : IFlat(nextE));
      | INested(l, _) =>
        let (nextL, nearestHostOutputNode) =
          List.fold_left(
            (
              (acc, nearestHostOutputNode: outputNodeContainer),
              renderedElement,
            ) => {
              let (nearestHostOutputNode, next) =
                fold(f, renderedElement, nearestHostOutputNode);
              ([next, ...acc], nearestHostOutputNode);
            },
            ([], nearestHostOutputNode),
            List.rev(l): list(instanceForest),
          );
        let unchanged = List.for_all2((===), l, nextL);

        (
          nearestHostOutputNode,
          unchanged ?
            renderedElement :
            INested(
              nextL,
              List.fold_left(
                (acc, elem) => getSubtreeSize(elem) + acc,
                0,
                nextL,
              ),
            ),
        );
      };
    let rec flatten =
      fun
      | IFlat(l) => [l]
      | INested(l, _) => ListTR.concat(ListTR.map(flatten, l));
    let outputTreeNodes = subtree =>
      flatten(subtree)
      |> List.fold_left(
           (
             acc: list(outputNodeContainer),
             Instance({component: {elementType}, hostInstance}),
           ) =>
             switch (elementType) {
             | React => List.append(hostInstance, acc)
             | Host => [hostInstance, ...acc]
             },
           [],
         )
      |> List.rev;
  };

  module SyntheticElement = {
    let rec map = (f, syntheticElement) =>
      switch (syntheticElement) {
      | Flat(l) => IFlat(f(l))
      | Nested(l) =>
        let renderedElements = ListTR.map(map(f), l);
        INested(
          renderedElements,
          List.map(InstanceForest.getSubtreeSize, renderedElements)
          |> List.fold_left((+), 0),
        );
      };
    let rec fold =
            (f, renderedElement, nearestHostOutputNode: outputNodeContainer) =>
      switch (renderedElement) {
      | Flat(e) =>
        let (nearestHostOutputNode, opaqueInstance) =
          f(~nearestHostOutputNode, e);
        {nearestHostOutputNode, instanceForest: IFlat(opaqueInstance)};
      | Nested(l) =>
        let (nextL, nearestHostOutputNode) =
          List.fold_left(
            ((acc, nearestHostOutputNode), element) => {
              let {nearestHostOutputNode, instanceForest} =
                fold(f, element, nearestHostOutputNode);
              ([instanceForest, ...acc], nearestHostOutputNode);
            },
            ([], nearestHostOutputNode),
            List.rev(l),
          );

        {
          nearestHostOutputNode,
          instanceForest:
            INested(
              nextL,
              List.map(InstanceForest.getSubtreeSize, nextL)
              |> List.fold_left((+), 0),
            ),
        };
      };
  };

  module Node = {
    let make:
      type a s typ outputNode.
        (
          elementType(typ, outputNode, s, a),
          typ,
          instanceForest,
          self(s, a)
        ) =>
        outputNode =
      (elementType, subElements, instanceSubTree, self) =>
        switch (elementType) {
        | Host =>
          lazy {
            let instance =
              subElements.make() |> subElements.updateInstance(self);
            Node(
              List.fold_left(
                ((position, parent), child) => (
                  position + 1,
                  {
                    let Node(child) | UpdatedNode(_, child) =
                      Lazy.force(child);
                    OutputTree.insertNode(~parent, ~child, ~position);
                  },
                ),
                (0, instance),
                InstanceForest.outputTreeNodes(instanceSubTree),
              )
              |> snd,
            );
          }
        | React => InstanceForest.outputTreeNodes(instanceSubTree)
        };
  };

  let defaultShouldUpdate = _oldNewSef => true;
  let defaultDidUpdate = _oldNewSef => ();
  let defaultDidMount = _self => ();
  let defaultWillUnmount = _self => ();

  module SubtreeChange = {
    let insertNodes =
        (
          ~parent as parentWrapper: internalOutputNode,
          ~children: outputNodeGroup,
          ~position as initialPosition: int,
        ) => {
      let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
      let newParent =
        List.fold_left(
          ((position, parent), child) => (
            position + 1,
            {
              let Node(child) | UpdatedNode(_, child) = Lazy.force(child);
              OutputTree.insertNode(~parent, ~child, ~position);
            },
          ),
          (initialPosition, oldParent),
          children,
        )
        |> snd;
      newParent === oldParent ?
        parentWrapper : UpdatedNode(oldParent, newParent);
    };
    let deleteNodes =
        (
          ~parent as parentWrapper: internalOutputNode,
          ~children: outputNodeGroup,
        ) => {
      let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
      let newParent =
        List.fold_left(
          (parent, child) => {
            let Node(child) | UpdatedNode(_, child) = Lazy.force(child);
            OutputTree.deleteNode(~parent, ~child);
          },
          oldParent,
          children,
        );
      newParent === oldParent ?
        parentWrapper : UpdatedNode(oldParent, newParent);
    };

    let prependElement =
        (~parent: outputNodeContainer, ~children: outputNodeGroup)
        : outputNodeContainer =>
      lazy (insertNodes(~parent=Lazy.force(parent), ~children, ~position=0));

    let replaceSubtree =
        (
          ~parent: outputNodeContainer,
          ~prevChildren: outputNodeGroup,
          ~nextChildren: outputNodeGroup,
        )
        : outputNodeContainer =>
      lazy {
        let parent = Lazy.force(parent);
        insertNodes(
          ~parent=deleteNodes(~parent, ~children=prevChildren),
          ~children=nextChildren,
          ~position=0,
        );
      };

    let reorderNode =
        (
          ~child: outputNodeContainer,
          ~parent: OutputTree.node,
          ~from: int,
          ~to_: int,
        ) =>
      switch (Lazy.force(child)) {
      | Node(child) => OutputTree.moveNode(~parent, ~child, ~from, ~to_)
      | UpdatedNode(prevChild, child) =>
        OutputTree.insertNode(
          ~parent=OutputTree.deleteNode(~parent, ~child=prevChild),
          ~child,
          ~position=to_,
        )
      };

    let reorder =
        (
          ~parent,
          ~instance as Instance({hostInstance, component: {elementType}}),
          ~from,
          ~to_,
        )
        : outputNodeContainer =>
      switch (elementType) {
      | Host =>
        lazy {
          let parentWrapper = Lazy.force(parent);
          let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
          let newParent =
            reorderNode(~parent=oldParent, ~child=hostInstance, ~from, ~to_);
          newParent === oldParent ?
            parentWrapper : UpdatedNode(oldParent, newParent);
        }
      | React =>
        lazy {
          let parentWrapper = Lazy.force(parent);
          let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
          let newParent =
            List.fold_left(
              ((index, parent), child) => (
                index + 1,
                reorderNode(
                  ~parent,
                  ~child,
                  ~from=from + index,
                  ~to_=to_ + index,
                ),
              ),
              (0, oldParent),
              hostInstance,
            )
            |> snd;
          newParent === oldParent ?
            parentWrapper : UpdatedNode(oldParent, newParent);
        }
      };

    let updateNodes = (~parent, ~instanceForest: instanceForest) =>
      lazy {
        let parentWrapper = Lazy.force(parent);
        let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
        let newParent =
          List.fold_left(
            (instance, x) =>
              switch (Lazy.force(x)) {
              | Node(_child) => instance
              | UpdatedNode(oldNode, newNode) =>
                OutputTree.insertNode(
                  ~parent=
                    OutputTree.deleteNode(~parent=instance, ~child=oldNode),
                  ~child=newNode,
                  ~position=0,
                )
              },
            oldParent,
            InstanceForest.outputTreeNodes(instanceForest),
          );
        newParent === oldParent ?
          parentWrapper : UpdatedNode(oldParent, newParent);
      };
  };

  module Self = {
    let make = (~state, ~component, ~pendingStateUpdates): self(_) => {
      state,
      reduce: (payloadToAction, payload) => {
        let action = payloadToAction(payload);
        let stateUpdate = component.reducer(action);
        OutputTree.markAsStale();
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      },
      act: action => {
        let stateUpdate = component.reducer(action);
        OutputTree.markAsStale();
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      },
    };
    let makeInitial = (~component) => {
      let id = GlobalState.instanceIdCounter^;
      incr(GlobalState.instanceIdCounter);
      let pendingStateUpdates = ref([]);
      (
        make(
          ~component,
          ~state=component.initialState(),
          ~pendingStateUpdates,
        ),
        pendingStateUpdates,
        id,
      );
    };
    let make = (~instance) =>
      make(
        ~component=instance.component,
        ~state=instance.iState,
        ~pendingStateUpdates=instance.pendingStateUpdates,
      );
  };

  module OpaqueInstanceHash = {
    let addOpaqueInstance = (idTable, index, opaqueInstance) => {
      let Instance({component: {key}}) = opaqueInstance;
      key == Key.none ?
        () : Hashtbl.add(idTable, key, (opaqueInstance, index));
    };
    let addRenderedElement = (idTable, renderedElement, index) => {
      let rec aux = index =>
        fun
        | IFlat(l) => addOpaqueInstance(idTable, index, l)
        | INested(l, _) => List.iteri((i, x) => aux(i, x), l);
      aux(index, renderedElement);
    };
    let createKeyTable = renderedElement =>
      lazy {
        let keyTable = Hashtbl.create(1);
        addRenderedElement(keyTable, renderedElement, 0);
        keyTable;
      };
    let lookupKey = (table, key) => {
      let keyTable = Lazy.force(table);
      try (Some(Hashtbl.find(keyTable, key))) {
      | Not_found => None
      };
    };
  };

  module Instance = {
    let rec ofElement = (Element(component) as element): opaqueInstance => {
      let (self, pendingStateUpdates, id) = Self.makeInitial(~component);
      /* Invoke didMount
         if (component.didMount !== defaultDidMount) {
           component.didMount(self);
         };
         */
      let subElements = component.render(self);
      let instanceSubTree =
        (
          switch (component.elementType) {
          | React => (subElements: syntheticElement)
          | Host => subElements.children
          }
        )
        |> ofList;
      Instance({
        id,
        iState: self.state,
        component,
        subElements,
        instanceSubTree,
        pendingStateUpdates,
        element,
        hostInstance:
          Node.make(
            component.elementType,
            subElements,
            instanceSubTree,
            self,
          ),
      });
    }
    and ofList = (syntheticElement): instanceForest =>
      SyntheticElement.map(ofElement, syntheticElement);
  };

  module Render = {
    let getOpaqueInstance = (~useKeyTable, Element({key})) =>
      switch (useKeyTable) {
      | None => None
      | Some(keyTable) => OpaqueInstanceHash.lookupKey(keyTable, key)
      };

    let executePendingStateUpdates:
      'state 'action 'elementType 'outputNode.
      instance('state, 'action, 'elementType, 'outputNode) =>
      instance('state, 'action, 'elementType, 'outputNode)
     =
      instance => {
        let executeUpdate = (~state, stateUpdate) =>
          switch (stateUpdate(state)) {
          | NoUpdate => state
          | Update(newState) => newState
          };
        let rec executeUpdates = (~state, stateUpdates) =>
          switch (stateUpdates) {
          | [] => state
          | [stateUpdate, ...otherStateUpdates] =>
            let nextState = executeUpdate(~state, stateUpdate);
            executeUpdates(~state=nextState, otherStateUpdates);
          };
        let pendingUpdates = List.rev(instance.pendingStateUpdates^);
        instance.pendingStateUpdates := [];
        let nextState =
          executeUpdates(~state=instance.iState, pendingUpdates);
        instance.iState === nextState ?
          instance : {...instance, iState: nextState};
      };

    type childElementUpdate = {
      updatedRenderedElement: renderedElement,
      /* This represents the way previously rendered elements have been shifted due to moves */
      indexShift: int,
    };

    /**
     * Initial render of an Element. Recurses to produce the entire tree of
     * instances.
     */
    let rec renderElement =
            (~useKeyTable=?, ~nearestHostOutputNode, element)
            : (outputNodeContainer, opaqueInstance) =>
      switch (getOpaqueInstance(~useKeyTable, element)) {
      | Some((opaqueInstance, _)) =>
        updateOpaqueInstance(~nearestHostOutputNode, opaqueInstance, element)
      | None => (nearestHostOutputNode, Instance.ofElement(element))
      }
    and renderReactElement =
        (~useKeyTable=?, nearestHostOutputNode, syntheticElement)
        : renderedElement =>
      SyntheticElement.fold(
        renderElement(~useKeyTable?),
        syntheticElement,
        nearestHostOutputNode,
      )
    /**
     * Update a previously rendered instance tree according to a new Element.
     *
     * Here's where the magic happens:
     * -------------------------------
     *
     * We perform a dynamic check that two types are statically equal by way of
     * mutation! We have a value of type `Instance` and another of type
     * `Element`, where we each has their own `component 'x` for potentially
     * different 'x. We need to see if the 'x are the same and if so safely "cast"
     * one's `component` to the others. We do this by handing off state safely into
     * one of their `component`s, and then seeing if we can observe it in the
     * other, and if so, we simply treat the old component as the new one's.
     *
     * This approach is as sound as our confidence in our ability to repair the
     * mutations accurately.
     *
     * There are more elegant ways to do this using first class modules, combined
     * with extensible variants. That is how we should do this if we want to turn
     * this implementation into something serious - it would avoid hitting the
     * write barrier.
     *
     * The UpdateLog:
     * ---------------------
     * The updates happen depth first and so the update log contains the deepes
     * changes first.
     * A change at depth N in the tree, causes all nodes from 0 to N generate an
     * update. It's because the render tree is an immutable data structure.
     * A change deep within a tree, means that the subtree of its parent has
     * changed and it propagates to the root of a tree.
     */
    and updateOpaqueInstance =
        (
          ~shouldExecutePendingUpdates=false,
          ~nearestHostOutputNode: outputNodeContainer,
          Instance(originalInstance) as originalOpaqueInstance,
          Element(nextComponent) as nextElement,
        )
        : (outputNodeContainer, opaqueInstance) => {
      let updatedInstance =
        shouldExecutePendingUpdates ?
          executePendingStateUpdates(originalInstance) : originalInstance;

      let stateChanged = originalInstance !== updatedInstance;

      let bailOut = {
        let Instance({element}) = originalOpaqueInstance;
        !stateChanged && element === nextElement;
      };

      if (bailOut) {
        (nearestHostOutputNode, originalOpaqueInstance);
      } else {
        let {component} = updatedInstance;
        component.handedOffInstance := Some(updatedInstance);
        switch (nextComponent.handedOffInstance^) {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some(updatedInstance) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          component.handedOffInstance := None;

          let (nearestHostOutputNode, newOpaqueInstance) as ret =
            updateInstance(
              ~originalOpaqueInstance,
              ~shouldExecutePendingUpdates,
              ~nearestHostOutputNode,
              ~nextComponent,
              ~nextElement,
              ~stateChanged,
              updatedInstance,
            );
          newOpaqueInstance === originalOpaqueInstance ?
            ret :
            (
              SubtreeChange.updateNodes(
                ~parent=nearestHostOutputNode,
                ~instanceForest=IFlat(newOpaqueInstance),
              ),
              newOpaqueInstance,
            );
        /*
         * Case B: The next element is *not* of the same component class. We know
         * because otherwise we would have observed the mutation on
         * `nextComponentClass`.
         */
        | None =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          component.handedOffInstance := None;
          /**
           * ** Switching component type **
           * TODO: Invoke willUnmount on previous component.
           */
          let opaqueInstance = Instance.ofElement(nextElement);
          (
            SubtreeChange.replaceSubtree(
              ~parent=nearestHostOutputNode,
              ~prevChildren=
                InstanceForest.outputTreeNodes(
                  IFlat(originalOpaqueInstance),
                ),
              ~nextChildren=
                InstanceForest.outputTreeNodes(IFlat(opaqueInstance)),
            ),
            opaqueInstance,
          );
        };
      };
    }
    and updateInstance:
      type state action elementType outputNodeType.
        (
          ~originalOpaqueInstance: opaqueInstance,
          ~shouldExecutePendingUpdates: bool,
          ~nearestHostOutputNode: outputNodeContainer,
          ~nextComponent: component(
                            state,
                            action,
                            elementType,
                            outputNodeType,
                          ),
          ~nextElement: element,
          ~stateChanged: bool,
          instance(state, action, elementType, outputNodeType)
        ) =>
        (outputNodeContainer, opaqueInstance) =
      (
        ~originalOpaqueInstance,
        ~shouldExecutePendingUpdates,
        ~nearestHostOutputNode,
        ~nextComponent,
        ~nextElement,
        ~stateChanged,
        instance,
      ) => {
        let updatedInstanceWithNewElement = {
          ...instance,
          component: nextComponent,
          element: nextElement,
        };
        let oldSelf = Self.make(~instance);
        let newState = nextComponent.willReceiveProps(oldSelf);

        /* We need to split up the check for state changes in two parts.
         * The first part covers pending updates.
         * The second part covers willReceiveProps.
         */
        let stateChanged = stateChanged || newState !== instance.iState;
        let updatedInstanceWithNewState = {
          ...updatedInstanceWithNewElement,
          iState: newState,
        };
        let newSelf = Self.make(~instance=updatedInstanceWithNewState);

        if (nextComponent.shouldUpdate === defaultShouldUpdate
            || nextComponent.shouldUpdate({oldSelf, newSelf})) {
          let nextSubElements = nextComponent.render(newSelf);
          let {subElements, instanceSubTree} = updatedInstanceWithNewState;
          /* TODO: Invoke didUpdate. */
          let (nearestHostOutputNode, updatedInstanceWithNewSubtree) =
            switch (nextComponent.elementType) {
            | React =>
              let {nearestHostOutputNode, instanceForest: nextInstanceSubtree} =
                updateInstanceSubtree(
                  ~shouldExecutePendingUpdates,
                  ~nearestHostOutputNode,
                  ~oldInstanceForest=instanceSubTree,
                  ~oldReactElement=subElements: syntheticElement,
                  ~nextReactElement=nextSubElements: syntheticElement,
                  (),
                );
              nextInstanceSubtree !== instanceSubTree ?
                (
                  nearestHostOutputNode,
                  {
                    ...updatedInstanceWithNewState,
                    subElements: nextSubElements,
                    instanceSubTree: nextInstanceSubtree,
                  },
                ) :
                (nearestHostOutputNode, updatedInstanceWithNewState);
            | Host =>
              let instanceWithNewHostView = {
                let shouldReconfigure =
                  nextSubElements.shouldReconfigureInstance(
                    ~oldState=oldSelf.state,
                    ~newState=newSelf.state,
                  );
                shouldReconfigure ?
                  {
                    ...updatedInstanceWithNewState,
                    hostInstance:
                      lazy {
                        let instance =
                          Lazy.force(
                            updatedInstanceWithNewState.hostInstance,
                          );
                        let Node(beforeUpdate) | UpdatedNode(_, beforeUpdate) = instance;
                        let afterUpdate =
                          nextSubElements.updateInstance(
                            newSelf,
                            beforeUpdate,
                          );
                        afterUpdate === beforeUpdate ?
                          instance : UpdatedNode(beforeUpdate, afterUpdate);
                      },
                  } :
                  updatedInstanceWithNewState;
              };

              let {
                nearestHostOutputNode: hostInstance,
                instanceForest: nextInstanceSubtree,
              } =
                updateInstanceSubtree(
                  ~shouldExecutePendingUpdates,
                  ~nearestHostOutputNode=instanceWithNewHostView.hostInstance: outputNodeContainer,
                  ~oldInstanceForest=instanceSubTree,
                  ~oldReactElement=subElements.children,
                  ~nextReactElement=nextSubElements.children,
                  (),
                );
              if (nextInstanceSubtree
                  !== instanceWithNewHostView.instanceSubTree) {
                (
                  /* FIXME AKI */
                  nearestHostOutputNode,
                  Obj.magic({
                    ...instanceWithNewHostView,
                    instanceSubTree: nextInstanceSubtree,
                    subElements: nextSubElements,
                    hostInstance,
                  }),
                );
              } else {
                (nearestHostOutputNode, instanceWithNewHostView);
              };
            };
          if (updatedInstanceWithNewSubtree === updatedInstanceWithNewState
              && !stateChanged) {
            (nearestHostOutputNode, originalOpaqueInstance);
          } else {
            (nearestHostOutputNode, Instance(updatedInstanceWithNewSubtree));
          };
        } else if (stateChanged) {
          (nearestHostOutputNode, Instance(updatedInstanceWithNewState));
        } else {
          (nearestHostOutputNode, originalOpaqueInstance);
        };
      }
    /**
     * updateRenderedElement recurses through the syntheticElement tree as long as
     * the oldReactElement and nextReactElement have the same shape.
     *
     * The base case is either an empty list - Nested([]) or a Flat element.
     *
     * syntheticElement is a recursive tree like data structure. The tree doesn't
     * contain children of the syntheticElements returned from children, it only
     * contains the "immediate" children so to speak including all nested lists.
     *
     * `keyTable` is a hash table containing all keys in the syntheticElement tree.
     */
    and updateInstanceSubtree =
        (
          ~shouldExecutePendingUpdates=?,
          ~useKeyTable=?,
          ~absoluteSubtreeIndex=?,
          ~nearestHostOutputNode: outputNodeContainer,
          ~oldInstanceForest,
          ~oldReactElement,
          ~nextReactElement,
          (),
        )
        : renderedElement =>
      switch (oldInstanceForest, oldReactElement, nextReactElement) {
      | (
          INested(instanceSubTrees, subtreeSize),
          Nested(oldReactElements),
          Nested([nextReactElement, ...nextReactElements]),
        )
          when
            nextReactElements === oldReactElements && GlobalState.useTailHack^ =>
        /* Detected that nextReactElement was obtained by adding one element */
        let {nearestHostOutputNode, instanceForest: addedElement} =
          renderReactElement(nearestHostOutputNode, nextReactElement);
        {
          nearestHostOutputNode:
            SubtreeChange.prependElement(
              ~parent=nearestHostOutputNode,
              ~children=InstanceForest.outputTreeNodes(addedElement),
            ),
          /*** Prepend element */
          instanceForest:
            INested(
              [addedElement, ...instanceSubTrees],
              subtreeSize + InstanceForest.getSubtreeSize(addedElement),
            ),
        };
      | (
          INested(oldInstanceForests, _),
          Nested(oldReactElements),
          Nested(nextReactElements),
        )
          when
            List.length(nextReactElements)
            === List.length(oldInstanceForests) =>
        let keyTable =
          switch (useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldInstanceForest)
          | Some(keyTable) => keyTable
          };
        let (nearestHostOutputNode, newInstanceForests, subtreeSize) =
          ListTR.fold3(
            (
              (nearestHostOutputNode, renderedElements, prevSubtreeSize),
              oldInstanceForest,
              oldReactElement,
              nextReactElement,
            ) => {
              let {
                indexShift,
                updatedRenderedElement: {
                  nearestHostOutputNode,
                  instanceForest,
                },
              } =
                updateChildRenderedElement(
                  ~shouldExecutePendingUpdates?,
                  ~useKeyTable=keyTable,
                  ~nearestHostOutputNode,
                  ~absoluteSubtreeIndex=prevSubtreeSize,
                  ~oldInstanceForest,
                  ~oldReactElement,
                  ~nextReactElement,
                  (),
                );
              (
                nearestHostOutputNode,
                [instanceForest, ...renderedElements],
                prevSubtreeSize
                + InstanceForest.getSubtreeSize(instanceForest)
                - indexShift,
              );
            },
            oldInstanceForests,
            oldReactElements,
            nextReactElements,
            (
              nearestHostOutputNode,
              [],
              switch (absoluteSubtreeIndex) {
              | Some(x) => x
              | None => 0
              },
            ),
          );
        let newInstanceForests = List.rev(newInstanceForests);
        {
          nearestHostOutputNode,
          instanceForest: INested(newInstanceForests, subtreeSize),
        };
      /*
       * Key Policy for syntheticElement.
       * Nested elements determine shape: if the shape is not identical, re-render.
       * Flat elements use a positional match by default, where components at
       * the same position (from left) are matched for updates.
       * If the component has an explicit key, match the instance with the same key.
       * Note: components are matched for key across the entire syntheticElement structure.
       */
      | (
          IFlat(oldOpaqueInstance),
          Flat(Element({key: oldKey})),
          Flat(Element({key: nextKey}) as nextReactElement),
        ) =>
        if (nextKey !== oldKey) {
          /* Not found: render a new instance */
          let (nearestHostOutputNode, newInstanceForest) =
            renderElement(nextReactElement, ~nearestHostOutputNode);
          let newInstanceForest = IFlat(newInstanceForest);
          {
            nearestHostOutputNode:
              SubtreeChange.replaceSubtree(
                ~parent=nearestHostOutputNode,
                ~prevChildren=
                  InstanceForest.outputTreeNodes(oldInstanceForest),
                ~nextChildren=
                  InstanceForest.outputTreeNodes(newInstanceForest),
              ),
            instanceForest: newInstanceForest,
          };
        } else {
          /* TODO: this should cause updateOpaqueInstance */
          let (nearestHostOutputNode, newOpaqueInstance) =
            updateOpaqueInstance(
              ~shouldExecutePendingUpdates?,
              ~nearestHostOutputNode,
              oldOpaqueInstance,
              nextReactElement,
            );
          {
            nearestHostOutputNode,
            instanceForest:
              oldOpaqueInstance !== newOpaqueInstance ?
                IFlat(newOpaqueInstance) : oldInstanceForest,
          };
        }
      | (_, _, _) =>
        /* Notice that all elements which are queried successfully
         *  from the hash table must have been here in the previous render
         * No, it's not true. What if the key is the same but element type changes
         * Wtf, stop thinking
         */
        let keyTable =
          switch (useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldInstanceForest)
          | Some(keyTable) => keyTable
          };
        let {nearestHostOutputNode, instanceForest: newInstanceForest} =
          renderReactElement(
            ~useKeyTable=keyTable,
            nearestHostOutputNode,
            nextReactElement,
          );
        {
          nearestHostOutputNode:
            SubtreeChange.replaceSubtree(
              ~parent=nearestHostOutputNode,
              ~prevChildren=InstanceForest.outputTreeNodes(oldInstanceForest),
              ~nextChildren=InstanceForest.outputTreeNodes(newInstanceForest),
            ),
          instanceForest: newInstanceForest,
        };
      }
    and updateChildRenderedElement =
        (
          ~shouldExecutePendingUpdates=?,
          ~useKeyTable=?,
          /* This is a unique index of an element within a subtree,
            * thanks to tracking it we can efficiently manage moves of within a subtree
           */
          ~nearestHostOutputNode,
          ~absoluteSubtreeIndex,
          ~oldInstanceForest,
          ~oldReactElement,
          ~nextReactElement,
          (),
        )
        : childElementUpdate =>
      switch (oldInstanceForest, oldReactElement, nextReactElement) {
      /*
       * Key Policy for syntheticElement.
       * Nested elements determine shape: if the shape is not identical, re-render.
       * Flat elements use a positional match by default, where components at
       * the same position (from left) are matched for updates.
       * If the component has an explicit key, match the instance with the same key.
       * Note: components are matched for key across the entire syntheticElement structure.
       */
      | (
          IFlat(oldOpaqueInstance),
          Flat(Element({key: oldKey})),
          Flat(Element({key: nextKey}) as nextReactElement),
        ) =>
        let keyTable =
          switch (useKeyTable) {
          | None =>
            OpaqueInstanceHash.createKeyTable(IFlat(oldOpaqueInstance))
          | Some(keyTable) => keyTable
          };
        let (nearestHostOutputNode, update, newOpaqueInstance) = {
          let Element(component) = nextReactElement;
          if (component.key !== Key.none) {
            switch (OpaqueInstanceHash.lookupKey(keyTable, component.key)) {
            | Some((subOpaqueInstance, previousIndex)) =>
              /* Instance tree with the same component key */
              let (nearestHostOutputNode, ins) =
                updateOpaqueInstance(
                  ~shouldExecutePendingUpdates?,
                  ~nearestHostOutputNode,
                  subOpaqueInstance,
                  nextReactElement,
                );
              (nearestHostOutputNode, `NoChangeOrNested(previousIndex), ins);
            | None =>
              /* Not found: render a new instance */
              let (nearestHostOutputNode, n) =
                renderElement(~nearestHostOutputNode, nextReactElement);
              (nearestHostOutputNode, `NewElement, n);
            };
          } else {
            let (nearestHostOutputNode, newOpaqueInstance) =
              updateOpaqueInstance(
                ~nearestHostOutputNode,
                oldOpaqueInstance,
                nextReactElement,
              );
            (nearestHostOutputNode, `NoChangeOrNested(0), newOpaqueInstance);
          };
        };
        switch (update) {
        | `NewElement =>
          let newInstanceForest = IFlat(newOpaqueInstance);
          {
            updatedRenderedElement: {
              nearestHostOutputNode:
                SubtreeChange.replaceSubtree(
                  ~parent=nearestHostOutputNode,
                  ~prevChildren=
                    InstanceForest.outputTreeNodes(oldInstanceForest),
                  ~nextChildren=
                    InstanceForest.outputTreeNodes(newInstanceForest),
                ),
              instanceForest: newInstanceForest,
            },
            indexShift: 0,
          };
        | `NoChangeOrNested(previousIndex) =>
          let changed = oldOpaqueInstance !== newOpaqueInstance;
          let element =
            changed ? IFlat(newOpaqueInstance) : oldInstanceForest;
          if (oldKey != nextKey) {
            {
              updatedRenderedElement: {
                nearestHostOutputNode:
                  previousIndex != absoluteSubtreeIndex ?
                    SubtreeChange.reorder(
                      ~parent=nearestHostOutputNode,
                      ~instance=newOpaqueInstance,
                      ~from=previousIndex,
                      ~to_=absoluteSubtreeIndex,
                    ) :
                    nearestHostOutputNode,
                instanceForest: element,
              },
              indexShift: InstanceForest.getSubtreeSize(element),
            };
          } else {
            {
              updatedRenderedElement: {
                nearestHostOutputNode:
                  if (changed) {
                    SubtreeChange.updateNodes(
                      ~parent=nearestHostOutputNode,
                      ~instanceForest=element,
                    );
                  } else {
                    nearestHostOutputNode;
                  },
                instanceForest: element,
              },
              indexShift: 0,
            };
          };
        };
      | (_, _, _) => {
          updatedRenderedElement:
            updateInstanceSubtree(
              ~absoluteSubtreeIndex,
              ~shouldExecutePendingUpdates?,
              ~nearestHostOutputNode,
              /* Not sure about this */
              ~useKeyTable?,
              ~oldInstanceForest,
              ~oldReactElement,
              ~nextReactElement,
              (),
            ),
          indexShift: 0,
        }
      };

    /**
     * Execute the pending updates at the top level of an instance tree.
     * If no state change is performed, the argument is returned unchanged.
     */
    /**
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = (opaqueInstance, nearestHostOutputNode) => {
      let Instance({element}) = opaqueInstance;
      updateOpaqueInstance(
        ~shouldExecutePendingUpdates=true,
        ~nearestHostOutputNode,
        opaqueInstance,
        element,
      );
    };
  };

  module RenderedElement = {
    /**
     * Rendering produces a list of instance trees.
     */
    type t = renderedElement;

    let listToRenderedElement = renderedElements =>
      INested(
        renderedElements,
        List.map(InstanceForest.getSubtreeSize, renderedElements)
        |> List.fold_left((+), 0),
      );
    let render = (nearestHostOutputNode: OutputTree.node, syntheticElement): t => {
      let instanceForest = Instance.ofList(syntheticElement);
      {
        instanceForest,
        nearestHostOutputNode:
          lazy (
            Node(
              InstanceForest.outputTreeNodes(instanceForest)
              |> List.fold_left(
                   ((position, parent), child) => (
                     position + 1,
                     {
                       let Node(child) | UpdatedNode(_, child) =
                         Lazy.force(child);
                       OutputTree.insertNode(~parent, ~child, ~position);
                     },
                   ),
                   (0, nearestHostOutputNode),
                 )
              |> snd,
            )
          ),
      };
    };
    let update =
        (
          ~previousElement,
          ~renderedElement as {instanceForest, nearestHostOutputNode}: t,
          nextReactElement,
        )
        : t =>
      Render.updateInstanceSubtree(
        ~oldInstanceForest=instanceForest,
        ~oldReactElement=previousElement,
        ~nearestHostOutputNode,
        ~nextReactElement,
        (),
      );

    /**
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = ({instanceForest, nearestHostOutputNode}: t): t => {
      let (nearestHostOutputNode, newInstanceForest) =
        InstanceForest.fold(
          Render.flushPendingUpdates,
          instanceForest,
          nearestHostOutputNode,
        );
      {instanceForest: newInstanceForest, nearestHostOutputNode};
    };

    let executeHostViewUpdates = ({nearestHostOutputNode}) => {
      OutputTree.beginChanges();
      let Node(hostView) | UpdatedNode(_, hostView) =
        Lazy.force(nearestHostOutputNode);
      OutputTree.commitChanges();
      hostView;
    };
  };

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

  /**
   * If `useDynamicKey` is true, every element of a given componentSpec
   * will have initial value of key equal to `Key.dynamicKeyMagicNumber`.
   *
   * For such cases, we generate a new key during calls to the `element` function.
   */
  let basicComponent = (~useDynamicKey=false, debugName, elementType) => {
    let key = useDynamicKey ? Key.dynamicKeyMagicNumber : Key.none;
    {
      debugName,
      elementType,
      willReceiveProps: ({state}) => state,
      didMount: defaultDidMount,
      didUpdate: defaultDidUpdate,
      willUnmount: defaultWillUnmount,
      shouldUpdate: defaultShouldUpdate,
      render: _self => assert(false),
      initialState: () => (),
      reducer: (_action, _state) => NoUpdate,
      printState: _state => "",
      handedOffInstance: ref(None),
      key,
    };
  };

  let statelessComponent:
    (~useDynamicKey: bool=?, string) =>
    component(stateless, actionless, syntheticElement, outputNodeGroup) =
    (~useDynamicKey=?, debugName) => {
      ...basicComponent(~useDynamicKey?, debugName, React),
      initialState: () => (),
    };
  let statefulComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec(
      'state,
      stateless,
      actionless,
      syntheticElement,
      outputNodeGroup,
    ) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let reducerComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec(
      'state,
      stateless,
      'action,
      syntheticElement,
      outputNodeGroup,
    ) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let statelessNativeComponent:
    (~useDynamicKey: bool=?, string) =>
    component(
      stateless,
      actionless,
      outputTreeElement(stateless, actionless),
      outputNodeContainer,
    ) =
    (~useDynamicKey=?, debugName) => {
      ...basicComponent(~useDynamicKey?, debugName, Host),
      initialState: () => (),
    };

  let statefulNativeComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec(
      'state,
      stateless,
      actionless,
      outputTreeElement('state, actionless),
      outputNodeContainer,
    ) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, Host);

  let element = (~key as argumentKey=Key.none, component) => {
    let key =
      argumentKey != Key.none ?
        argumentKey :
        {
          let isDynamicKey = component.key == Key.dynamicKeyMagicNumber;
          isDynamicKey ? Key.create() : Key.none;
        };
    let componentWithKey =
      key != component.key ? {...component, key} : component;
    Flat(Element(componentWithKey));
  };
  let listToElement = l => Nested(l);

  module RemoteAction = {
    type t('action) = {mutable act: 'action => unit};
    let actDefault = _action => ();
    let create = () => {act: actDefault};
    let subscribe = (~act, x) =>
      if (x.act === actDefault) {
        x.act = act;
      };
    let act = (x, ~action) => x.act(action);
  };
};
