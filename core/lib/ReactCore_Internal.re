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
      instanceIdCounter := 0;
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
  type internalOutputNode =
    | Node(OutputTree.node)
    | UpdatedNode(OutputTree.node, OutputTree.node);
  type outputNodeContainer = Lazy.t(internalOutputNode);
  type outputNodeGroup = list(outputNodeContainer);
  type instance('slots, 'nextSlots, 'elementType, 'outputNode) = {
    slots: Hooks.t('slots, 'nextSlots),
    component: component('slots, 'nextSlots, 'elementType, 'outputNode),
    element,
    instanceSubForest: instanceForest,
    subElements: 'elementType,
    hostInstance: 'outputNode,
  }
  and element =
    | Element(component('slots, 'nextSlots, 'elementType, 'outputNode))
      : element
  and syntheticElement =
    | Flat(element)
    | Nested(list(syntheticElement))
  and outputTreeElement('slots, 'nextSlots) = {
    make: unit => OutputTree.node,
    configureInstance:
      (~isFirstRender: bool, OutputTree.node) => OutputTree.node,
    children: syntheticElement,
  }
  and elementType('slots, 'nextSlots, 'elementType, 'outputNode) =
    | Host: elementType(
              'slots,
              'nextSlots,
              outputTreeElement('slots, 'nextSlots),
              outputNodeContainer,
            )
    | React: elementType(
               'slots,
               'nextSlots,
               syntheticElement,
               outputNodeGroup,
             )
  and instanceForest =
    | IFlat(opaqueInstance)
    | INested(list(instanceForest), int /*subtree size*/)
  and component('slots, 'nextSlots, 'elementType, 'outputNode) = {
    key: int,
    elementType: elementType('slots, 'nextSlots, 'elementType, 'outputNode),
    handedOffInstance:
      ref(option(instance('slots, 'nextSlots, 'elementType, 'outputNode))),
    render: Hooks.t('slots, 'nextSlots) => 'elementType,
  }
  and opaqueInstance =
    | Instance(instance('slots, 'nextSlots, 'elementType, 'outputNode))
      : opaqueInstance;

  type renderedElement = {
    nearestHostOutputNode: outputNodeContainer,
    instanceForest,
    enqueuedEffects: list(unit => unit),
  };

  type opaqueInstanceUpdate = {
    nearestHostOutputNode: outputNodeContainer,
    opaqueInstance,
    enqueuedEffects: list(unit => unit),
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
        let {nearestHostOutputNode, opaqueInstance, enqueuedEffects} =
          f(e, nearestHostOutputNode);
        let unchanged = e === opaqueInstance;

        {
          nearestHostOutputNode,
          instanceForest: unchanged ? renderedElement : IFlat(opaqueInstance),
          enqueuedEffects,
        };
      | INested(l, _) =>
        let (nextL, nearestHostOutputNode, effects) =
          List.fold_left(
            (
              (acc, nearestHostOutputNode: outputNodeContainer, effectsAcc),
              renderedElement,
            ) => {
              let {
                nearestHostOutputNode,
                instanceForest: next,
                enqueuedEffects,
              } =
                fold(f, renderedElement, nearestHostOutputNode);
              (
                [next, ...acc],
                nearestHostOutputNode,
                List.append(effectsAcc, enqueuedEffects),
              );
            },
            ([], nearestHostOutputNode, []),
            List.rev(l): list(instanceForest),
          );
        let unchanged = List.for_all2((===), l, nextL);

        {
          nearestHostOutputNode,
          instanceForest:
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
          enqueuedEffects: effects,
        };
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
      | Flat(l) =>
        let (opaqueInstance, enqueuedMountEffects) = f(l);
        (IFlat(opaqueInstance), enqueuedMountEffects);
      | Nested(l) =>
        let instanceSubForestAndEffects = ListTR.map(map(f), l);
        let subForest = ListTR.map(fst, instanceSubForestAndEffects);
        let effects = ListTR.map(snd, instanceSubForestAndEffects);
        (
          INested(
            subForest,
            subForest
            |> List.map(InstanceForest.getSubtreeSize)
            |> List.fold_left((+), 0),
          ),
          List.concat(effects),
        );
      };
    let rec fold =
            (f, renderedElement, nearestHostOutputNode: outputNodeContainer) =>
      switch (renderedElement) {
      | Flat(e) =>
        let {nearestHostOutputNode, opaqueInstance, enqueuedEffects} =
          f(~nearestHostOutputNode, e);
        {
          nearestHostOutputNode,
          instanceForest: IFlat(opaqueInstance),
          enqueuedEffects,
        };
      | Nested(l) =>
        let (nextL, nearestHostOutputNode, enqueuedEffects) =
          List.fold_left(
            ((acc, nearestHostOutputNode, enqueuedEffects), element) => {
              let {nearestHostOutputNode, instanceForest} =
                fold(f, element, nearestHostOutputNode);
              (
                [instanceForest, ...acc],
                nearestHostOutputNode,
                enqueuedEffects,
              );
            },
            ([], nearestHostOutputNode, []),
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
          enqueuedEffects,
        };
      };
  };

  module Node = {
    let make:
      type slots nextSlots elementType_ outputNode.
        (
          elementType(slots, nextSlots, elementType_, outputNode),
          elementType_,
          instanceForest
        ) =>
        outputNode =
      (elementType, subElements, instanceSubTree) =>
        switch (elementType) {
        | Host =>
          lazy {
            let instance =
              subElements.make()
              |> subElements.configureInstance(~isFirstRender=true);
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
          ~indexShift: int,
          ~from: int,
          ~to_: int,
        ) => {
      let isVal = Lazy.is_val(child);
      switch (Lazy.force(child)) {
      | Node(child) =>
        from === to_ - indexShift ?
          parent : OutputTree.moveNode(~parent, ~child, ~from, ~to_)
      | UpdatedNode(prevChild, child) when !isVal =>
        OutputTree.insertNode(
          ~parent=OutputTree.deleteNode(~parent, ~child=prevChild),
          ~child,
          ~position=to_,
        )
      | UpdatedNode(_prevChild, child) =>
        from === to_ - indexShift ?
          parent : OutputTree.moveNode(~parent, ~child, ~from, ~to_)
      };
    };

    let reorder =
        (
          ~parent,
          ~instance as Instance({hostInstance, component: {elementType}}),
          ~indexShift,
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
            reorderNode(
              ~parent=oldParent,
              ~child=hostInstance,
              ~indexShift,
              ~from,
              ~to_,
            );
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
                  ~indexShift,
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

    let updateNodes =
        (
          ~parent,
          ~instanceForest: instanceForest,
          ~position as initialPosition,
        ) =>
      lazy {
        let parentWrapper = Lazy.force(parent);
        let Node(oldParent) | UpdatedNode(_, oldParent) = parentWrapper;
        let newParent =
          List.fold_left(
            ((position, instance), x) => (
              position + 1,
              switch (Lazy.force(x)) {
              | Node(_child) => instance
              | UpdatedNode(oldNode, newNode) =>
                OutputTree.insertNode(
                  ~parent=
                    OutputTree.deleteNode(~parent=instance, ~child=oldNode),
                  ~child=newNode,
                  ~position,
                )
              },
            ),
            (initialPosition, oldParent),
            InstanceForest.outputTreeNodes(instanceForest),
          )
          |> snd;
        newParent === oldParent ?
          parentWrapper : UpdatedNode(oldParent, newParent);
      };
  };

  module OpaqueInstanceHash = {
    type t = lazy_t(Hashtbl.t(int, (opaqueInstance, int)));
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
    let rec ofElement =
            (Element(component) as element)
            : (opaqueInstance, list(unit => unit)) => {
      let slots = Hooks.create(~onSlotsDidChange=OutputTree.markAsStale);
      let subElements = component.render(slots);
      let (instanceSubForest, mountEffects) =
        (
          switch (component.elementType) {
          | React => (subElements: syntheticElement)
          | Host => subElements.children
          }
        )
        |> ofList;
      (
        Instance({
          slots,
          element,
          component,
          subElements,
          instanceSubForest,
          hostInstance:
            Node.make(component.elementType, subElements, instanceSubForest),
        }),
        [
          () =>
            ignore(
              Hooks.executeEffects(~lifecycle=Hooks.Effect.Mount, slots),
            ),
          ...mountEffects,
        ],
      );
    }
    and ofList = (syntheticElement): (instanceForest, list(unit => unit)) =>
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
      instance('state, 'action, 'elementType, 'outputNode) => bool
     =
      instance => Hooks.flushPendingStateUpdates(instance.slots);

    type childElementUpdate = {
      updatedRenderedElement: renderedElement,
      /* This represents the way previously rendered elements have been shifted due to moves */
      indexShift: int,
    };

    module UpdateContext = {
      type t = {
        shouldExecutePendingUpdates: bool,
        useKeyTable: option(OpaqueInstanceHash.t),
        /* This is a unique index of an element within a subtree,
          * thanks to tracking it we can efficiently manage moves of within a subtree
         */
        nearestHostOutputNode: outputNodeContainer,
        absoluteSubtreeIndex: int,
      };
    };

    /**
     * Initial render of an Element. Recurses to produce the entire tree of
     * instances.
     */
    let rec renderElement =
            (~useKeyTable=?, ~nearestHostOutputNode, element)
            : opaqueInstanceUpdate =>
      switch (getOpaqueInstance(~useKeyTable, element)) {
      | Some((opaqueInstance, _)) =>
        updateOpaqueInstance(
          ~updateContext=
            UpdateContext.{
              nearestHostOutputNode,
              absoluteSubtreeIndex: 0,
              useKeyTable,
              shouldExecutePendingUpdates: false,
            },
          opaqueInstance,
          element,
        )
      | None =>
        let (opaqueInstance, enqueuedEffects) = Instance.ofElement(element);

        {nearestHostOutputNode, opaqueInstance, enqueuedEffects};
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
          ~updateContext: UpdateContext.t,
          Instance(instance) as originalOpaqueInstance,
          Element(nextComponent) as nextElement,
        )
        : opaqueInstanceUpdate => {
      let stateChanged =
        updateContext.shouldExecutePendingUpdates ?
          executePendingStateUpdates(instance) : false;

      let bailOut = !stateChanged && instance.element === nextElement;

      if (bailOut && !updateContext.shouldExecutePendingUpdates) {
        {
          nearestHostOutputNode: updateContext.nearestHostOutputNode,
          opaqueInstance: originalOpaqueInstance,
          enqueuedEffects: [],
        };
      } else {
        let {component} = instance;
        component.handedOffInstance := Some(instance);
        switch (nextComponent.handedOffInstance^) {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some(handedInstance) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          component.handedOffInstance := None;

          let {
                nearestHostOutputNode,
                opaqueInstance: newOpaqueInstance,
                enqueuedEffects,
              } as ret =
            updateInstance(
              ~originalOpaqueInstance,
              ~updateContext,
              ~nextComponent,
              ~nextElement,
              ~stateChanged,
              handedInstance,
            );
          newOpaqueInstance === originalOpaqueInstance ?
            ret :
            {
              nearestHostOutputNode:
                SubtreeChange.updateNodes(
                  ~parent=nearestHostOutputNode,
                  ~instanceForest=IFlat(newOpaqueInstance),
                  ~position=updateContext.absoluteSubtreeIndex,
                ),
              opaqueInstance: newOpaqueInstance,
              enqueuedEffects,
            };
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
           */
          let {enqueuedEffects: unmountEffects}: renderedElement =
            InstanceForest.fold(
              (Instance({slots}) as opaqueInstance, nearestHostOutputNode) => {
                nearestHostOutputNode,
                opaqueInstance,
                enqueuedEffects: [
                  () =>
                    ignore(
                      Hooks.executeEffects(
                        ~lifecycle=Hooks.Effect.Unmount,
                        slots,
                      ),
                    ),
                ],
              },
              instance.instanceSubForest,
              updateContext.nearestHostOutputNode,
            );
          let unmountEffects = [
            (
              () => {
                ignore(
                  Hooks.executeEffects(
                    ~lifecycle=Hooks.Effect.Unmount,
                    instance.slots,
                  ),
                );
              }
            ),
            ...unmountEffects,
          ];
          let (opaqueInstance, mountEffects) =
            Instance.ofElement(nextElement);
          {
            nearestHostOutputNode:
              SubtreeChange.replaceSubtree(
                ~parent=updateContext.nearestHostOutputNode,
                ~prevChildren=
                  InstanceForest.outputTreeNodes(
                    IFlat(originalOpaqueInstance),
                  ),
                ~nextChildren=
                  InstanceForest.outputTreeNodes(IFlat(opaqueInstance)),
              ),
            opaqueInstance,
            enqueuedEffects: List.append(unmountEffects, mountEffects),
          };
        };
      };
    }
    and updateInstance:
      type slots nextSlots elementType outputNodeType.
        (
          ~originalOpaqueInstance: opaqueInstance,
          ~updateContext: UpdateContext.t,
          ~nextComponent: component(
                            slots,
                            nextSlots,
                            elementType,
                            outputNodeType,
                          ),
          ~nextElement: element,
          ~stateChanged: bool,
          instance(slots, nextSlots, elementType, outputNodeType)
        ) =>
        opaqueInstanceUpdate =
      (
        ~originalOpaqueInstance,
        ~updateContext,
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

        let shouldRerender = stateChanged || nextElement !== instance.element;

        let nextSubElements =
          shouldRerender ?
            nextComponent.render(updatedInstanceWithNewElement.slots) :
            instance.subElements;
        let {subElements, instanceSubForest} = updatedInstanceWithNewElement;
        let (
          nearestHostOutputNode,
          updatedInstanceWithNewSubtree,
          enqueuedEffects,
        ) =
          switch (nextComponent.elementType) {
          | React =>
            let {
              nearestHostOutputNode,
              instanceForest: nextInstanceSubForest,
              enqueuedEffects,
            } =
              updateInstanceSubtree(
                ~updateContext,
                ~oldInstanceForest=instanceSubForest,
                ~oldReactElement=subElements: syntheticElement,
                ~nextReactElement=nextSubElements: syntheticElement,
                (),
              );
            nextInstanceSubForest !== instanceSubForest ?
              (
                nearestHostOutputNode,
                {
                  ...updatedInstanceWithNewElement,
                  subElements: nextSubElements,
                  instanceSubForest: nextInstanceSubForest,
                },
                enqueuedEffects,
              ) :
              (
                nearestHostOutputNode,
                updatedInstanceWithNewElement,
                enqueuedEffects,
              );
          | Host =>
            let instanceWithNewHostView =
              shouldRerender ?
                {
                  ...updatedInstanceWithNewElement,
                  hostInstance:
                    lazy {
                      let instance =
                        Lazy.force(
                          updatedInstanceWithNewElement.hostInstance,
                        );
                      let Node(beforeUpdate) | UpdatedNode(_, beforeUpdate) = instance;
                      let afterUpdate =
                        nextSubElements.configureInstance(
                          ~isFirstRender=false,
                          beforeUpdate,
                        );
                      afterUpdate === beforeUpdate ?
                        instance : UpdatedNode(beforeUpdate, afterUpdate);
                    },
                } :
                updatedInstanceWithNewElement;

            let {
              nearestHostOutputNode: hostInstance,
              instanceForest: nextInstanceSubForest,
              enqueuedEffects,
            } =
              updateInstanceSubtree(
                ~updateContext={
                  ...updateContext,
                  absoluteSubtreeIndex: 0,
                  nearestHostOutputNode: (
                    instanceWithNewHostView.hostInstance: outputNodeContainer
                  ),
                },
                ~oldInstanceForest=instanceSubForest,
                ~oldReactElement=subElements.children,
                ~nextReactElement=nextSubElements.children,
                (),
              );
            if (nextInstanceSubForest
                !== instanceWithNewHostView.instanceSubForest) {
              (
                updateContext.nearestHostOutputNode,
                {
                  ...instanceWithNewHostView,
                  instanceSubForest: nextInstanceSubForest,
                  subElements: nextSubElements,
                  hostInstance,
                }:
                  instance(slots, nextSlots, elementType, outputNodeType),
                enqueuedEffects,
              );
            } else {
              (
                updateContext.nearestHostOutputNode,
                instanceWithNewHostView,
                enqueuedEffects,
              );
            };
          };
        if (updatedInstanceWithNewSubtree === updatedInstanceWithNewElement
            && !stateChanged) {
          {
            nearestHostOutputNode,
            opaqueInstance: originalOpaqueInstance,
            enqueuedEffects,
          };
        } else {
          {
            nearestHostOutputNode,
            opaqueInstance: Instance(updatedInstanceWithNewSubtree),
            enqueuedEffects: [
              () =>
                ignore(
                  Hooks.executeEffects(
                    ~lifecycle=Hooks.Effect.Update,
                    updatedInstanceWithNewSubtree.slots,
                  ),
                ),
              ...enqueuedEffects,
            ],
          };
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
          ~updateContext,
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
        let {
          nearestHostOutputNode,
          instanceForest: addedElement,
          enqueuedEffects,
        } =
          renderReactElement(
            updateContext.nearestHostOutputNode,
            nextReactElement,
          );
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
          enqueuedEffects,
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
          switch (updateContext.useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldInstanceForest)
          | Some(keyTable) => keyTable
          };
        let (
          nearestHostOutputNode,
          newInstanceForests,
          subtreeSize,
          _indexShift,
          enqueuedEffects,
        ) =
          ListTR.fold3(
            (
              (
                nearestHostOutputNode,
                renderedElements,
                prevSubtreeSize,
                indexShift,
                enqueuedEffectsAcc,
              ),
              oldInstanceForest,
              oldReactElement,
              nextReactElement,
            ) => {
              let {
                indexShift,
                updatedRenderedElement: {
                  nearestHostOutputNode,
                  instanceForest,
                  enqueuedEffects,
                },
              } =
                updateChildRenderedElement(
                  ~updateContext={
                    ...updateContext,
                    nearestHostOutputNode,
                    useKeyTable: Some(keyTable),
                    absoluteSubtreeIndex: prevSubtreeSize,
                  },
                  ~indexShift,
                  ~oldInstanceForest,
                  ~oldReactElement,
                  ~nextReactElement,
                  (),
                );
              (
                nearestHostOutputNode,
                [instanceForest, ...renderedElements],
                prevSubtreeSize
                + InstanceForest.getSubtreeSize(instanceForest),
                indexShift,
                List.append(enqueuedEffects, enqueuedEffectsAcc),
              );
            },
            oldInstanceForests,
            oldReactElements,
            nextReactElements,
            (
              updateContext.nearestHostOutputNode,
              [],
              updateContext.absoluteSubtreeIndex,
              0,
              [],
            ),
          );
        let newInstanceForests = List.rev(newInstanceForests);
        {
          nearestHostOutputNode,
          instanceForest: INested(newInstanceForests, subtreeSize),
          enqueuedEffects,
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
          let {
            nearestHostOutputNode,
            opaqueInstance: newOpaqueInstance,
            enqueuedEffects,
          } =
            renderElement(
              nextReactElement,
              ~nearestHostOutputNode=updateContext.nearestHostOutputNode,
            );
          let newInstanceForest = IFlat(newOpaqueInstance);
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
            enqueuedEffects,
          };
        } else {
          let {
            nearestHostOutputNode,
            opaqueInstance: newOpaqueInstance,
            enqueuedEffects,
          } =
            updateOpaqueInstance(
              ~updateContext={...updateContext, useKeyTable: None},
              oldOpaqueInstance,
              nextReactElement,
            );
          {
            nearestHostOutputNode,
            instanceForest:
              oldOpaqueInstance !== newOpaqueInstance ?
                IFlat(newOpaqueInstance) : oldInstanceForest,
            enqueuedEffects,
          };
        }
      | (_, _, _) =>
        /* Notice that all elements which are queried successfully
         *  from the hash table must have been here in the previous render
         * No, it's not true. What if the key is the same but element type changes
         * Wtf, stop thinking
         */
        let keyTable =
          switch (updateContext.useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldInstanceForest)
          | Some(keyTable) => keyTable
          };
        let {
          nearestHostOutputNode,
          instanceForest: newInstanceForest,
          enqueuedEffects,
        } =
          renderReactElement(
            ~useKeyTable=keyTable,
            updateContext.nearestHostOutputNode,
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
          enqueuedEffects,
        };
      }
    and updateChildRenderedElement =
        (
          ~updateContext as {
            UpdateContext.shouldExecutePendingUpdates,
            useKeyTable,
            nearestHostOutputNode,
            absoluteSubtreeIndex,
          },
          ~indexShift,
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
        let (
          nearestHostOutputNode,
          update,
          newOpaqueInstance,
          enqueuedEffects,
        ) = {
          let Element(component) = nextReactElement;
          if (component.key !== Key.none) {
            switch (OpaqueInstanceHash.lookupKey(keyTable, component.key)) {
            | Some((subOpaqueInstance, previousIndex)) =>
              /* Instance tree with the same component key */
              let {
                nearestHostOutputNode,
                opaqueInstance: updatedOpaqueInstance,
                enqueuedEffects,
              } =
                updateOpaqueInstance(
                  ~updateContext=
                    UpdateContext.{
                      useKeyTable: None,
                      shouldExecutePendingUpdates,
                      nearestHostOutputNode,
                      absoluteSubtreeIndex: previousIndex + indexShift,
                    },
                  subOpaqueInstance,
                  nextReactElement,
                );
              (
                nearestHostOutputNode,
                `NoChangeOrNested(previousIndex),
                updatedOpaqueInstance,
                enqueuedEffects,
              );
            | None =>
              /* Not found: render a new instance */
              let {
                nearestHostOutputNode,
                opaqueInstance: newOpaqueInstance,
                enqueuedEffects,
              } =
                renderElement(~nearestHostOutputNode, nextReactElement);
              (
                nearestHostOutputNode,
                `NewElement,
                newOpaqueInstance,
                enqueuedEffects,
              );
            };
          } else {
            let {
              nearestHostOutputNode,
              opaqueInstance: updatedOpaqueInstance,
              enqueuedEffects,
            } =
              updateOpaqueInstance(
                ~updateContext=
                  UpdateContext.{
                    shouldExecutePendingUpdates,
                    nearestHostOutputNode,
                    absoluteSubtreeIndex,
                    useKeyTable: None,
                  },
                oldOpaqueInstance,
                nextReactElement,
              );
            (
              nearestHostOutputNode,
              `NoChangeOrNested(absoluteSubtreeIndex),
              updatedOpaqueInstance,
              enqueuedEffects,
            );
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
              enqueuedEffects,
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
                  SubtreeChange.reorder(
                    ~parent=nearestHostOutputNode,
                    ~instance=newOpaqueInstance,
                    ~indexShift,
                    ~from=previousIndex,
                    ~to_=absoluteSubtreeIndex,
                  ),
                instanceForest: element,
                enqueuedEffects,
              },
              indexShift: InstanceForest.getSubtreeSize(element),
            };
          } else {
            {
              updatedRenderedElement: {
                nearestHostOutputNode,

                instanceForest: element,
                enqueuedEffects,
              },
              indexShift: 0,
            };
          };
        };
      | (_, _, _) => {
          updatedRenderedElement:
            updateInstanceSubtree(
              ~updateContext={
                UpdateContext.absoluteSubtreeIndex,
                shouldExecutePendingUpdates,
                nearestHostOutputNode,
                /* Not sure about this */
                useKeyTable,
              },
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
    let flushPendingUpdates = (opaqueInstance, nearestHostOutputNode) => {
      let Instance({element}) = opaqueInstance;
      updateOpaqueInstance(
        ~updateContext=
          UpdateContext.{
            useKeyTable: None,
            shouldExecutePendingUpdates: true,
            nearestHostOutputNode,
            absoluteSubtreeIndex: 0,
          },
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
        renderedElements
        |> List.fold_left(
             (acc, e) => acc + InstanceForest.getSubtreeSize(e),
             0,
           ),
      );
    let render = (nearestHostOutputNode: OutputTree.node, syntheticElement): t => {
      let (instanceForest, mountEffects) = Instance.ofList(syntheticElement);
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
                       let parent =
                         OutputTree.insertNode(~parent, ~child, ~position);
                       parent;
                     },
                   ),
                   (0, nearestHostOutputNode),
                 )
              |> snd,
            )
          ),
        enqueuedEffects: mountEffects,
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
        ~updateContext=
          Render.UpdateContext.{
            nearestHostOutputNode,
            absoluteSubtreeIndex: 0,
            useKeyTable: None,
            shouldExecutePendingUpdates: false,
          },
        ~oldInstanceForest=instanceForest,
        ~oldReactElement=previousElement,
        ~nextReactElement,
        (),
      );

    /**
     * Flush the pending updates in an instance tree.
     */
    let flushPendingUpdates =
        ({instanceForest, nearestHostOutputNode, enqueuedEffects}: t): t => {
      let {
        nearestHostOutputNode,
        instanceForest: newInstanceForest,
        enqueuedEffects: nextEnqueuedEffects,
      } =
        InstanceForest.fold(
          Render.flushPendingUpdates,
          instanceForest,
          nearestHostOutputNode,
        );
      {
        instanceForest: newInstanceForest,
        nearestHostOutputNode,
        enqueuedEffects: List.append(nextEnqueuedEffects, enqueuedEffects),
      };
    };

    let executeHostViewUpdates = ({nearestHostOutputNode}: t) => {
      OutputTree.beginChanges();
      let Node(hostView) | UpdatedNode(_, hostView) =
        Lazy.force(nearestHostOutputNode);
      OutputTree.commitChanges();
      hostView;
    };

    let executePendingEffects = ({enqueuedEffects} as renderedElement: t) => {
      List.iter(f => f(), enqueuedEffects);
      {...renderedElement, enqueuedEffects: []};
    };
  };

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

  /* Temporary mechanism for keeping identity without the first class module */
  let component = (~useDynamicKey=false, _debugName) => {
    let handedOffInstance = ref(None);
    render => {
      elementType: React,
      key: useDynamicKey ? Key.dynamicKeyMagicNumber : Key.none,
      handedOffInstance,
      render,
    };
  };

  let nativeComponent = (~useDynamicKey=false, _debugName) => {
    let handedOffInstance = ref(None);
    render => {
      elementType: Host,
      key: useDynamicKey ? Key.dynamicKeyMagicNumber : Key.none,
      handedOffInstance,
      render,
    };
  };

  module Slots = Slots;
  module Hooks = Hooks;
  module RemoteAction = RemoteAction;
};
