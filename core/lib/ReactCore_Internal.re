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

module type HostImplementation = {
  type hostView;

  let markAsDirty: unit => unit;

  let beginChanges: unit => unit;

  let mountChild:
    (~parent: hostView, ~child: hostView, ~position: int) => hostView;
  let unmountChild: (~parent: hostView, ~child: hostView) => hostView;
  let remountChild:
    (~parent: hostView, ~child: hostView, ~from: int, ~to_: int) => hostView;

  let commitChanges: unit => unit;
};

module Make = (Implementation: HostImplementation) => {
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

  type hostOutputNode = Lazy.t(Implementation.hostView);
  type syntheticOutputNode = list(Lazy.t(Implementation.hostView));

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
  and reactElement =
    | Flat(element)
    | Nested(list(reactElement))
  and nativeElement('state, 'action) = {
    make: unit => Implementation.hostView,
    updateInstance: (self('state, 'action), Implementation.hostView) => unit,
    shouldReconfigureInstance: (~oldState: 'state, ~newState: 'state) => bool,
    children: reactElement,
  }
  and elementType('renderedType, 'outputNode, 'state, 'action) =
    | Host: elementType(
              nativeElement('state, 'action),
              hostOutputNode,
              'state,
              'action,
            )
    | React: elementType(reactElement, syntheticOutputNode, 'state, 'action)
  and renderedElement =
    | IFlat(opaqueInstance)
    | INested(list(renderedElement), int /*subtree size*/)
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
     * This may not be a good idea to hold onto for sake of memory, but it makes
     * it convenient to implement shouldComponentUpdate.
     */
    element,
    iState: 'state,
    /**
     * Most recent subtree of instances.
     */
    instanceSubTree: renderedElement,
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
     * top secret
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

  let getSubtreeSize =
    fun
    | IFlat(_) => 0
    | INested(_, x) => x;
  let getSubtreeSizeL = elems =>
    List.fold_left((acc, elem) => getSubtreeSize(elem) + acc, 0, elems);
  let rec flattenReactElement =
    fun
    | Flat(l) => [l]
    | Nested(l) => ListTR.concat(ListTR.map(flattenReactElement, l));
  let rec flattenRenderedElement =
    fun
    | IFlat(l) => [l]
    | INested(l, _) => ListTR.concat(ListTR.map(flattenRenderedElement, l));
  let rec mapReactElement = (f, reactElement) =>
    switch (reactElement) {
    | Flat(l) => IFlat(f(l))
    | Nested(l) =>
      let renderedElements = ListTR.map(mapReactElement(f), l);
      INested(
        renderedElements,
        List.map(getSubtreeSize, renderedElements) |> List.fold_left((+), 0),
      );
    };
  let rec mapRenderedElement = (f, renderedElement) =>
    switch (renderedElement) {
    | IFlat(e) =>
      let nextE = f(e);
      let unchanged = e === nextE;
      unchanged ? renderedElement : IFlat(nextE);
    | INested(l, _) =>
      let nextL = ListTR.map(mapRenderedElement(f), l);
      let subtreeSize = getSubtreeSizeL(nextL);
      let unchanged = List.for_all2((===), l, nextL);
      unchanged ? renderedElement : INested(nextL, subtreeSize);
    };

  module OutputTree = {
    module InstanceSubtree = {
      let toList = subtree =>
        flattenRenderedElement(subtree)
        |> List.fold_left(
             (
               acc: list(hostOutputNode),
               Instance({component: {elementType}, hostInstance}),
             ) =>
               switch (elementType) {
               | React => List.append(hostInstance, acc)
               | Host => [hostInstance, ...acc]
               },
             [],
           );
    };

    module Node = {
      let make:
        type a s typ outputNode.
          (elementType(typ, outputNode, s, a), typ, renderedElement) =>
          outputNode =
        (elementType, subElements, instanceSubTree) =>
          switch (elementType) {
          | Host =>
            lazy {
              let instance = subElements.make();
              subElements.updateInstance(Obj.magic(), instance);
              snd(
                InstanceSubtree.toList(instanceSubTree)
                |> List.fold_left(
                     ((position, parent), child) => (
                       position + 1,
                       Implementation.mountChild(
                         ~parent,
                         ~child=Lazy.force(child),
                         ~position,
                       ),
                     ),
                     (0, instance),
                   ),
              );
            }
          | React => InstanceSubtree.toList(instanceSubTree)
          };
    };
    /*
     let reorder =
         (
           ~parent: opaqueInstance,
           ~child: opaqueInstance,
           ~from: int,
           ~to_: int,
         ) =>
       ();
       */
  };

  let logString = txt => GlobalState.debug^ ? print_endline(txt) : ();
  let defaultShouldUpdate = _oldNewSef => true;
  let defaultDidUpdate = _oldNewSef => ();
  let defaultDidMount = _self => ();
  let defaultWillUnmount = _self => ();

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

  module SubtreeChange = {
    type t = [
      | `NoChange
      | `Nested
      | `PrependElement(renderedElement)
      | `ReplaceSubtree(renderedElement, renderedElement)
      | `UpdateGroup(list(updateGroupChange))
    ]
    and updateGroupChange = [
      | `Reordered(int, int)
      | `ReorderedAndNestedChange(int, int)
      | `TopLevelChange(t)
    ];

    let rec mapRenderedElement = (f, element) =>
      switch (element) {
      | IFlat(l) => [f(l)]
      | INested(l, _) =>
        ListTR.concat(ListTR.map(mapRenderedElement(f), l))
      };

    let apply:
      type a s typ outputNode.
        (
          ~instance: instance(s, a, typ, outputNode),
          ~nearestHostOutputNode: hostOutputNode,
          (t, renderedElement)
        ) =>
        instance(s, a, typ, outputNode) =
      (
        ~instance as
          {component: {elementType}, hostInstance: prevHostInstance} as instance,
        ~nearestHostOutputNode,
        (subtreeChange, nextInstanceSubTree),
      ) =>
        switch (subtreeChange) {
        | `NoChange => instance
        /*
         * iterate over the instance subtree doing force and return self
         */
        | `Nested =>
          let children =
            OutputTree.InstanceSubtree.toList(nextInstanceSubTree);
          switch (elementType) {
          | React => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance: children,
            }
          | Host => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance:
                lazy {
                  let instance = Lazy.force(prevHostInstance);
                  List.iter(x => ignore(Lazy.force(x)), children);
                  instance;
                },
            }
          };
        /*
         * Just create new OutputTree.t and mount it at the beginning
         */
        | `PrependElement(renderedElement) =>
          let children = OutputTree.InstanceSubtree.toList(renderedElement);
          switch (elementType) {
          | React => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance:
                List.append(
                  List.map(
                    child =>
                      lazy (
                        Implementation.mountChild(
                          ~parent=Lazy.force(nearestHostOutputNode),
                          ~child=Lazy.force(child),
                          ~position=0,
                        )
                      ),
                    children,
                  ),
                  instance.hostInstance,
                ),
            }
          | Host => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance:
                lazy {
                  let prevInstance = Lazy.force(prevHostInstance);
                  List.fold_left(
                    (instance, child) =>
                      Implementation.mountChild(
                        ~parent=instance,
                        ~child=Lazy.force(child),
                        ~position=0,
                      ),
                    prevInstance,
                    children,
                  );
                },
            }
          };
        | `ReplaceSubtree(prev, next) =>
        /* TODO: START HERE */
          let prevChildren = OutputTree.InstanceSubtree.toList(prev);
          let nextChildren = OutputTree.InstanceSubtree.toList(next);
          switch (elementType) {
          | React => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance:
                List.map(
                  child =>
                    lazy (
                      Implementation.mountChild(
                        ~parent=Lazy.force(nearestHostOutputNode),
                        ~child=Lazy.force(child),
                        ~position=0,
                      )
                    ),
                  children,
                ),
            }
          | Host => {
              ...instance,
              instanceSubTree: nextInstanceSubTree,
              hostInstance:
                lazy {
                  let prevInstance = Lazy.force(prevHostInstance);
                  List.fold_left(
                    (instance, child) =>
                      Implementation.mountChild(
                        ~parent=instance,
                        ~child=Lazy.force(child),
                        ~position=0,
                      ),
                    prevInstance,
                    children,
                  );
                },
            }
          };

        | `UpdateGroup(_l) => instance
        }
    and applyGroupChange:
      type a s typ outputNode.
        (
          ~instance: instance(s, a, typ, outputNode),
          ~nearestHostOutputNode: hostOutputNode,
          (updateGroupChange, renderedElement)
        ) =>
        instance(s, a, typ, outputNode) =
      (
        ~instance,
        ~nearestHostOutputNode,
        (subtreeChange, nextInstanceSubTree),
      ) => instance;
  };

  module Render = {
    let createSelf = (~state, ~component, ~pendingStateUpdates): self(_) => {
      state,
      reduce: (payloadToAction, payload) => {
        let action = payloadToAction(payload);
        let stateUpdate = component.reducer(action);
        Implementation.markAsDirty();
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      },
      act: action => {
        let stateUpdate = component.reducer(action);
        Implementation.markAsDirty();
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      },
    };
    let createInitialSelf = (~component) => {
      let id = GlobalState.instanceIdCounter^;
      incr(GlobalState.instanceIdCounter);
      let pendingStateUpdates = ref([]);
      (
        createSelf(
          ~component,
          ~state=component.initialState(),
          ~pendingStateUpdates,
        ),
        pendingStateUpdates,
        id,
      );
    };
    let createSelf = (~instance) =>
      createSelf(
        ~component=instance.component,
        ~state=instance.iState,
        ~pendingStateUpdates=instance.pendingStateUpdates,
      );

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

    let getOpaqueInstance = (~useKeyTable, Element({key})) =>
      switch (useKeyTable) {
      | None => None
      | Some(keyTable) => OpaqueInstanceHash.lookupKey(keyTable, key)
      };

    /**
     * Used because of higher order polymorphism. See here 5.3:
     * https://caml.inria.fr/pub/docs/manual-ocaml/polymorphism.html
     *
     * When we pass the updateInstanceState function it has to be polymorphic
     * *for all* instance('state, 'action, 'elementType) and passing such
     * function as an argument introduces higher rank polymorphism.
     * It's because it's not the function itself has to be polymorphic but its
     * argument. Therefore we have to wrap it in a record
     */
    type updateInstanceState = {
      f:
        'state 'action 'elementType 'outputNode.
        instance('state, 'action, 'elementType, 'outputNode) =>
        instance('state, 'action, 'elementType, 'outputNode),

    };

    module Instance = {
      let rec ofElement = (Element(component) as element): opaqueInstance => {
        let (self, pendingStateUpdates, id) = createInitialSelf(~component);
        /*
         if (component.didMount !== defaultDidMount) {
           component.didMount(self);
         };
         */
        let subElements = component.render(self);
        let instanceSubTree =
          switch (component.elementType) {
          | React => ofList(subElements: reactElement)
          | Host => ofList(subElements.children)
          };
        Instance({
          id,
          iState: self.state,
          component,
          subElements,
          instanceSubTree,
          pendingStateUpdates,
          element,
          hostInstance:
            OutputTree.Node.make(
              component.elementType,
              subElements,
              instanceSubTree,
            ),
        });
      }
      and ofList = (reactElement): renderedElement =>
        mapReactElement(ofElement, reactElement);
    };

    /**
     * Initial render of an Element. Recurses to produce the entire tree of
     * instances.
     */
    let rec renderElement =
            (~useKeyTable=?, ~nearestHostOutputNode, element): opaqueInstance =>
      switch (getOpaqueInstance(~useKeyTable, element)) {
      | Some((opaqueInstance, _)) =>
        update(~nearestHostOutputNode, opaqueInstance, element)
      | None => Instance.ofElement(element)
      }
    and renderReactElement =
        (~useKeyTable=?, ~nearestHostOutputNode, reactElement)
        : renderedElement =>
      mapReactElement(
        renderElement(~useKeyTable?, ~nearestHostOutputNode),
        reactElement,
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
    and update =
        (
          ~updateInstanceState=?,
          ~nearestHostOutputNode: hostOutputNode,
          originalOpaqueInstance,
          Element(nextComponent) as nextElement,
        )
        : opaqueInstance => {
      let Instance(originalInstance) = originalOpaqueInstance;
      let updatedInstance =
        switch (updateInstanceState) {
        | Some({f}) => f(originalInstance)
        | None => originalInstance
        };
      let stateNotUpdated = originalInstance === updatedInstance;

      let bailOut = {
        let Instance({element}) = originalOpaqueInstance;
        stateNotUpdated && element === nextElement;
      };

      if (bailOut) {
        originalOpaqueInstance;
      } else {
        let {component: updatedComponent} = updatedInstance;
        updatedComponent.handedOffInstance := Some(updatedInstance);
        switch (nextComponent.handedOffInstance^) {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some(updatedInstance) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          updatedComponent.handedOffInstance := None;
          let updatedInstanceWithNewElement = {
            ...updatedInstance,
            component: nextComponent,
            element: nextElement,
          };
          let oldSelf = createSelf(~instance=updatedInstance);
          let newState = nextComponent.willReceiveProps(oldSelf);

          /** We need to split up the check for state changes in two parts.
           * The first part covers pending updates.
           * The second part covers willReceiveProps.
           */
          let stateChanged =
            !stateNotUpdated || newState !== updatedInstance.iState;
          let updatedInstanceWithNewState = {
            ...updatedInstanceWithNewElement,
            iState: newState,
          };
          /* TODO: Invoke any lifecycles necessary. */
          let newSelf = createSelf(~instance=updatedInstanceWithNewState);
          if (nextComponent.shouldUpdate === defaultShouldUpdate
              || nextComponent.shouldUpdate({oldSelf, newSelf})) {
            let nextSubElements = nextComponent.render(newSelf);
            let {subElements, instanceSubTree} = updatedInstanceWithNewState;
            let updatedInstanceWithNewSubtree =
              switch (nextComponent.elementType) {
              | React =>
                updateRenderedElementTopLevel(
                  ~updateInstanceState?,
                  ~nearestHostOutputNode,
                  ~oldRenderedElement=instanceSubTree,
                  ~oldReactElement=subElements: reactElement,
                  ~nextReactElement=nextSubElements: reactElement,
                  (),
                )
                |> SubtreeChange.apply(
                     ~nearestHostOutputNode,
                     ~instance=updatedInstanceWithNewState,
                   )
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
                          let prevHostInstance =
                            Lazy.force(
                              updatedInstanceWithNewState.hostInstance,
                            );
                          /* Add more complex updates here, too */
                          subElements.updateInstance(
                            newSelf,
                            prevHostInstance,
                          );
                          prevHostInstance;
                        },
                    } :
                    updatedInstanceWithNewState;
                };
                let nearestHostOutputNode: hostOutputNode =
                  instanceWithNewHostView.hostInstance;
                updateRenderedElementTopLevel(
                  ~updateInstanceState?,
                  ~nearestHostOutputNode,
                  ~oldRenderedElement=instanceSubTree,
                  ~oldReactElement=subElements.children,
                  ~nextReactElement=nextSubElements.children,
                  (),
                )
                |> SubtreeChange.apply(
                     ~nearestHostOutputNode,
                     ~instance=instanceWithNewHostView,
                   );
              };
            if (updatedInstanceWithNewSubtree === updatedInstanceWithNewState
                && !stateChanged) {
              originalOpaqueInstance;
            } else {
              Instance(updatedInstanceWithNewSubtree);
            };
          } else if (stateChanged) {
            Instance(updatedInstanceWithNewState);
          } else {
            originalOpaqueInstance;
          };
        /*
         * Case B: The next element is *not* of the same component class. We know
         * because otherwise we would have observed the mutation on
         * `nextComponentClass`.
         */
        | None =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          updatedComponent.handedOffInstance := None;
          /**
           * ** Switching component type **
           * TODO: Invoke destruction lifecycle on previous component.
           * TODO: Invoke creation lifecycle on next component.
           */
          Instance.ofElement(nextElement);
        };
      };
    }
    /**
     * updateRenderedElement recurses through the reactElement tree as long as
     * the oldReactElement and nextReactElement have the same shape.
     *
     * The base case is either an empty list - Nested([]) or a Flat element.
     *
     * reactElement is a recursive tree like data structure. The tree doesn't
     * contain children of the reactElements returned from children, it only
     * contains the "immediate" children so to speak including all nested lists.
     *
     * `keyTable` is a hash table containing all keys in the reactElement tree.
     */
    and updateRenderedElementTopLevel =
        (
          ~updateInstanceState=?,
          ~useKeyTable=?,
          ~absoluteSubtreeIndex=?,
          ~nearestHostOutputNode: hostOutputNode,
          ~oldRenderedElement,
          ~oldReactElement,
          ~nextReactElement,
          (),
        )
        : (SubtreeChange.t, renderedElement) =>
      switch (oldRenderedElement, oldReactElement, nextReactElement) {
      | (
          INested(instanceSubTrees, subtreeSize),
          Nested(oldReactElements),
          Nested([nextReactElement, ...nextReactElements]),
        )
          when
            nextReactElements === oldReactElements && GlobalState.useTailHack^ =>
        /* Detected that nextReactElement was obtained by adding one element */
        let addedElement =
          renderReactElement(~nearestHostOutputNode, nextReactElement);

        /** Prepend element */
        (
          `PrependElement(addedElement),
          INested([addedElement, ...instanceSubTrees], subtreeSize + 1),
        );
      | (
          INested(oldRenderedElements, _),
          Nested(oldReactElements),
          Nested(nextReactElements),
        )
          when
            List.length(nextReactElements)
            === List.length(oldRenderedElements) =>
        let keyTable =
          switch (useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldRenderedElement)
          | Some(keyTable) => keyTable
          };
        let (updatesAndNewRenderedElements, subtreeSize) =
          ListTR.fold3(
            (
              (updates, prevSubtreeSize),
              oldRenderedElement,
              oldReactElement,
              nextReactElement,
            ) => {
              let (change, renderedElement, subtreeSize) =
                updateChildRenderedElement(
                  ~updateInstanceState?,
                  ~useKeyTable=keyTable,
                  ~nearestHostOutputNode,
                  ~absoluteSubtreeIndex=
                    switch (absoluteSubtreeIndex) {
                    | Some(x) => prevSubtreeSize + x
                    | None => prevSubtreeSize
                    },
                  ~oldRenderedElement,
                  ~oldReactElement,
                  ~nextReactElement,
                  (),
                );
              (
                [(change, renderedElement), ...updates],
                prevSubtreeSize + subtreeSize,
              );
            },
            oldRenderedElements,
            oldReactElements,
            nextReactElements,
            ([], 0),
          );
        let updatesAndNewRenderedElements =
          List.rev(updatesAndNewRenderedElements);
        (
          `UpdateGroup(List.map(fst, updatesAndNewRenderedElements)),
          INested(List.map(snd, updatesAndNewRenderedElements), subtreeSize),
        );
      /*
       * Key Policy for reactElement.
       * Nested elements determine shape: if the shape is not identical, re-render.
       * Flat elements use a positional match by default, where components at
       * the same position (from left) are matched for updates.
       * If the component has an explicit key, match the instance with the same key.
       * Note: components are matched for key across the entire reactElement structure.
       */
      | (
          IFlat(oldOpaqueInstance),
          Flat(Element({key: oldKey})),
          Flat(Element({key: nextKey}) as nextReactElement),
        ) =>
        if (nextKey !== oldKey) {
          /* Not found: render a new instance */
          let newRenderedElement =
            IFlat(renderElement(~nearestHostOutputNode, nextReactElement));
          (
            `ReplaceSubtree((oldRenderedElement, newRenderedElement)),
            newRenderedElement,
          );
        } else {
          let newOpaqueInstance =
            update(
              ~updateInstanceState?,
              ~nearestHostOutputNode,
              oldOpaqueInstance,
              nextReactElement,
            );
          oldOpaqueInstance !== newOpaqueInstance ?
            (`Nested, IFlat(newOpaqueInstance)) :
            (`NoChange, oldRenderedElement);
        }
      | (_, _, _) =>
        /* Notice that all elements which are queried successfully
         *  from the hash table must have been here in the previous render
         * No, it's not true. What if the key is the same but element type changes
         * Wtf, stop thinking
         */
        let keyTable =
          switch (useKeyTable) {
          | None => OpaqueInstanceHash.createKeyTable(oldRenderedElement)
          | Some(keyTable) => keyTable
          };
        let newRenderedElement =
          renderReactElement(
            ~useKeyTable=keyTable,
            ~nearestHostOutputNode,
            nextReactElement,
          );
        /* It's completely new, replaceElement */
        (
          `ReplaceSubtree((oldRenderedElement, newRenderedElement)),
          newRenderedElement,
        );
      }
    and updateChildRenderedElement =
        (
          ~updateInstanceState=?,
          ~useKeyTable=?,
          /* This is a unique index of an element within a subtree,
            * thanks to tracking it we can efficiently manage moves of within a subtree
           */
          ~nearestHostOutputNode: hostOutputNode,
          ~absoluteSubtreeIndex,
          ~oldRenderedElement,
          ~oldReactElement,
          ~nextReactElement,
          (),
        )
        : (SubtreeChange.updateGroupChange, renderedElement, int) =>
      switch (oldRenderedElement, oldReactElement, nextReactElement) {
      /*
       * Key Policy for reactElement.
       * Nested elements determine shape: if the shape is not identical, re-render.
       * Flat elements use a positional match by default, where components at
       * the same position (from left) are matched for updates.
       * If the component has an explicit key, match the instance with the same key.
       * Note: components are matched for key across the entire reactElement structure.
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
        let (update, newOpaqueInstance) = {
          let Element(component) = nextReactElement;
          if (component.key !== Key.none) {
            switch (OpaqueInstanceHash.lookupKey(keyTable, component.key)) {
            | Some((subOpaqueInstance, previousIndex)) =>
              /* Instance tree with the same component key */
              (
                `NoChangeOrNested(previousIndex),
                update(
                  ~updateInstanceState?,
                  ~nearestHostOutputNode,
                  subOpaqueInstance,
                  nextReactElement,
                ),
              )
            | None =>
              /* Not found: render a new instance */
              (
                `NewElement,
                renderElement(~nearestHostOutputNode, nextReactElement),
              )
            };
          } else {
            (
              `NoChangeOrNested(0),
              update(
                ~updateInstanceState?,
                ~nearestHostOutputNode,
                oldOpaqueInstance,
                nextReactElement,
              ),
            );
          };
        };
        switch (update) {
        | `NewElement =>
          let newRenderedElement = IFlat(newOpaqueInstance);
          (
            `TopLevelChange(
              `ReplaceSubtree((oldRenderedElement, newRenderedElement)),
            ),
            newRenderedElement,
            1,
          );
        | `NoChangeOrNested(previousIndex) =>
          let changed = oldOpaqueInstance !== newOpaqueInstance;
          let element =
            changed ? IFlat(newOpaqueInstance) : oldRenderedElement;
          if (oldKey != nextKey) {
            (
              changed ?
                `ReorderedAndNestedChange((
                  previousIndex,
                  absoluteSubtreeIndex,
                )) :
                `Reordered((previousIndex, absoluteSubtreeIndex)),
              element,
              1,
            );
          } else {
            changed ?
              (`TopLevelChange(`Nested), element, 1) :
              (`TopLevelChange(`NoChange), element, 1);
          };
        };
      | (_, _, _) =>
        let (subtreeChange, renderedElement) =
          updateRenderedElementTopLevel(
            ~absoluteSubtreeIndex,
            ~updateInstanceState?,
            ~nearestHostOutputNode,
            /* Not sure about this */
            ~useKeyTable?,
            ~oldRenderedElement,
            ~oldReactElement,
            ~nextReactElement,
            (),
          );
        (
          `TopLevelChange(subtreeChange),
          renderedElement,
          getSubtreeSize(renderedElement),
        );
      };

    /**
     * Execute the pending updates at the top level of an instance tree.
     * If no state change is performed, the argument is returned unchanged.
     */
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

    /**
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = opaqueInstance => {
      let Instance({element}) = opaqueInstance;
      update(
        ~updateInstanceState={f: executePendingStateUpdates},
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

    let getSubtreeSize =
      fun
      | INested(_, x) => x
      | IFlat(_) => 1;

    let listToRenderedElement = renderedElements =>
      INested(
        renderedElements,
        List.map(getSubtreeSize, renderedElements) |> List.fold_left((+), 0),
      );
    let render = (reactElement): t => Render.Instance.ofList(reactElement);
    type subtreeChange = SubtreeChange.t;
    let update =
        (~previousReactElement, ~renderedElement: t, nextReactElement)
        : (SubtreeChange.t, t) => {
      let (change, elem) =
        Render.updateRenderedElementTopLevel(
          ~oldRenderedElement=renderedElement,
          ~oldReactElement=previousReactElement,
          ~nextReactElement,
          (),
        );
      (change, elem);
    };

    /**
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = (renderedElement: t): t => {
      let newRenderedElement =
        mapRenderedElement(Render.flushPendingUpdates, renderedElement);
      newRenderedElement;
    };
  };

  let statelessComponent:
    (~useDynamicKey: bool=?, string) =>
    component(stateless, actionless, reactElement, syntheticOutputNode) =
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
      reactElement,
      syntheticOutputNode,
    ) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let reducerComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec(
      'state,
      stateless,
      'action,
      reactElement,
      syntheticOutputNode,
    ) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let statelessNativeComponent:
    (~useDynamicKey: bool=?, string) =>
    component(
      stateless,
      actionless,
      nativeElement(stateless, actionless),
      hostOutputNode,
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
      nativeElement('state, actionless),
      hostOutputNode,
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
