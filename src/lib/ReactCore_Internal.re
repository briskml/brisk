open Flex;

module type HostImplementation = {
  type hostView;
  let getInstance: int => option(hostView);
  let memoizeInstance: (int, hostView) => unit;
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
    /*
     * Use physical equality to recognize that an element was added to the list
     * of children.
     * Note: this currently does not check for pending updates on components in
     * the list.
     */
    let useTailHack = ref(false);
  };
  module Key = {
    type t = int;
    let none = (-1);
    let first = 0;
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
  type reduce('payload, 'action) =
    ('payload => 'action) => Callback.t('payload);
  type update('state, 'action) =
    | NoUpdate
    | Update('state)
  and self('state, 'action) = {
    state: 'state,
    reduce: 'payload .reduce('payload, 'action),
    act: 'action => unit
  }
  /***
   * Elements are what JSX blocks become. They represent the *potential* for a
   * component instance and state to be created / updated. They are not yet
   * instances.
   */
  and element =
    | Element(component('state, 'action, 'elementType)): element
  /***
   * We will want to replace this with a more efficient data structure.
   */
  and reactElement =
    | Flat(list(element))
    | Nested(string, list(reactElement))
  and nativeElement = {
    make: unit => Implementation.hostView,
    updateInstance: Implementation.hostView => unit,
    children: reactElement
  }
  and elementType('a) =
    | Host: elementType(nativeElement)
    | React: elementType(reactElement)
  and renderedElement =
    | IFlat(list(opaqueInstance))
    | INested(string, list(renderedElement))
  and oldNewSelf('state, 'action) = {
    oldSelf: self('state, 'action),
    newSelf: self('state, 'action)
  }
  /**
    * This type is used to avoid Obj.magic in update.
    * We create one handedOffInstance ref per component type.
    * The componentSpec is usually reacreated on every render.
    * The ref is created in functions like `statelessComponent`. Then a user
    * usually spreads this ref like so:
    * let make = ... => {...component, ...};
    * Therefore all components coming from a given `make` function usually
    * share the same ref. Then, in the update function it can be determined
    *
    * If two components are of the same types when we passi the instance to one
    * component and we can observe the mutation on the other component,
    * it means that we've got two components of the same type.
    *
    * Additionally, we pass two instances here.
    * - The first instance in the tuple contains the original instance that
    *   the update has been started with
    * - The second instance is the same instance but after executing pending
    *   state updates
    */
  and handedOffInstance('state, 'action, 'element) =
    ref(
      option(
        (
          instance('state, 'action, 'element),
          instance('state, 'action, 'element)
        )
      )
    )
  and componentSpec('state, 'initialState, 'action, 'element) = {
    debugName: string,
    elementType: elementType('element),
    willReceiveProps: self('state, 'action) => 'state,
    didMount: self('state, 'action) => unit,
    didUpdate: oldNewSelf('state, 'action) => unit,
    willUnmount: self('state, 'action) => unit /* TODO: currently unused */,
    willUpdate: oldNewSelf('state, 'action) => unit,
    shouldUpdate: oldNewSelf('state, 'action) => bool,
    render: self('state, 'action) => 'element,
    initialState: unit => 'initialState,
    reducer: ('action, 'state) => update('state, 'action),
    printState: 'state => string /* for internal debugging */,
    handedOffInstance: handedOffInstance('state, 'action, 'element),
    key: int
  }
  and component('state, 'action, 'elementType) =
    componentSpec('state, 'state, 'action, 'elementType)
  /***
   * Elements turn into instances at the right time. These instances record the
   * most recent state among other things.
   */
  and instance('state, 'action, 'elementType) = {
    component: component('state, 'action, 'elementType),
    /***
     * This may not be a good idea to hold onto for sake of memory, but it makes
     * it convenient to implement shouldComponentUpdate.
     */
    element,
    iState: 'state,
    /***
     * Most recent subtree of instances.
     */
    instanceSubTree: renderedElement,
    subElements: 'elementType,
    /***
     * List of state updates pending for this instance.
     * Stored in reverse order.
     */
    pendingStateUpdates: ref(list('state => update('state, 'action))),
    /***
     * Unique instance id.
     * */
    id: int
  }
  /***
   * Opaque wrapper around `instance`, which allows many instances to be
   * commingled in a single data structure. The GADT hides the type parameters.
   * The result of "rendering" an Element, is a tree of instances that are
   * produced. This tree is then updated to produce a new *version* of the
   * instance tree based on the old - the old one is not mutated.
   */
  and opaqueInstance =
    | Instance(instance('state, 'action, 'elementType)): opaqueInstance;
  let logString = txt =>
    GlobalState.debug^ ?
      {
        print_string(txt);
        print_newline();
      } :
      ();
  let defaultShouldUpdate = _oldNewSef => true;
  let defaultWillUpdate = _oldNewSef => ();
  let defaultDidUpdate = _oldNewSef => ();
  let defaultDidMount = _self => ();
  let basicComponent = (~useDynamicKey=false, debugName, elementType) => {
    let key = useDynamicKey ? Key.first : Key.none;
    {
      debugName,
      elementType,
      willReceiveProps: ({state}) => state,
      didMount: defaultDidMount,
      didUpdate: defaultDidUpdate,
      willUnmount: _self => (),
      willUpdate: defaultWillUpdate,
      shouldUpdate: defaultShouldUpdate,
      render: _self => assert false,
      initialState: () => (),
      reducer: (_action, _state) => NoUpdate,
      printState: _state => "",
      handedOffInstance: ref(None),
      key
    };
  };

  /*** Tail-recursive functions on lists */
  module ListTR = {
    let useTailRecursion = l =>
      switch l {
      | [_, _, _, _, _, _, _, _, _, _, ..._] => true
      | _ => false
      };
    let concat = list => {
      let rec aux = (acc, l) =>
        switch l {
        | [] => List.rev(acc)
        | [x, ...rest] => aux(List.rev_append(x, acc), rest)
        };
      useTailRecursion(list) ? aux([], list) : List.concat(list);
    };
    let map = (f, list) =>
      useTailRecursion(list) ?
        List.rev_map(f, List.rev(list)) : List.map(f, list);
    let rev_map3 = (f, list1, list2, list3) => {
      let rec aux = acc =>
        fun
        | ([], [], []) => acc
        | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) =>
          aux([f((x1, x2, x3)), ...acc], (nextList1, nextList2, nextList3))
        | _ => assert false;
      aux([], (list1, list2, list3));
    };
    let map3 = (f, list1, list2, list3) =>
      rev_map3(f, List.rev(list1), List.rev(list2), List.rev(list3));
  };

  /*** Log of operations performed to update an instance tree. */
  module UpdateLog = {
    type subtreeChange = [
      | `Nested
      | `NoChange
      | `PrependElement(renderedElement)
      | `ReplaceElements(renderedElement, renderedElement)
    ];
    type instanceUpdate('state, 'action, 'elementType) = {
      oldInstance: instance('state, 'action, 'elementType),
      newInstance: instance('state, 'action, 'elementType),
      stateChanged: bool,
      subTreeChanged: subtreeChange
    };
    type componentChange = {
      oldId: int,
      newId: int,
      oldOpaqueInstance: opaqueInstance,
      newOpaqueInstance: opaqueInstance
    };
    type entry =
      | UpdateInstance(instanceUpdate('state, 'action, 'elementType)): entry
      | ChangeComponent(componentChange): entry;
    type t = ref(list(entry));
    let create = () => ref([]);
    let add = (updateLog, x) => updateLog := [x, ...updateLog^];
    let addUpdateInstance =
        (~updateLog, ~stateChanged, ~subTreeChanged, oldInstance, newInstance) =>
      add(
        updateLog,
        UpdateInstance({
          oldInstance,
          newInstance,
          stateChanged,
          subTreeChanged
        })
      );
    let addChangeComponent = (~updateLog, opaqueInstance, newOpaqueInstance) => {
      let Instance({id}) = opaqueInstance;
      let Instance({id: newId}) = newOpaqueInstance;
      add(
        updateLog,
        ChangeComponent({
          oldId: id,
          newId,
          oldOpaqueInstance: opaqueInstance,
          newOpaqueInstance
        })
      );
    };
  };
  module Render = {
    let createSelf = (~state, ~component, ~pendingStateUpdates) : self(_) => {
      state,
      reduce: (payloadToAction, payload) => {
        let action = payloadToAction(payload);
        let stateUpdate = component.reducer(action);
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      },
      act: action => {
        let stateUpdate = component.reducer(action);
        pendingStateUpdates := [stateUpdate, ...pendingStateUpdates^];
      }
    };
    let createInitialSelf = (~component) => {
      let id = GlobalState.instanceIdCounter^;
      incr(GlobalState.instanceIdCounter);
      let pendingStateUpdates = ref([]);
      (
        createSelf(
          ~component,
          ~state=component.initialState(),
          ~pendingStateUpdates
        ),
        pendingStateUpdates,
        id
      );
    };
    let createSelf = (~instance) =>
      createSelf(
        ~component=instance.component,
        ~state=instance.iState,
        ~pendingStateUpdates=instance.pendingStateUpdates
      );
    let rec flattenReactElement =
      fun
      | Flat(l) => l
      | Nested(_, l) => ListTR.concat(ListTR.map(flattenReactElement, l));
    let rec flattenRenderedElement =
      fun
      | IFlat(l) => l
      | INested(_, l) => ListTR.concat(ListTR.map(flattenRenderedElement, l));
    let rec mapReactElement = (f, reactElement) =>
      switch reactElement {
      | Flat(l) => IFlat(ListTR.map(f, l))
      | Nested(s, l) => INested(s, ListTR.map(mapReactElement(f), l))
      };
    let rec mapRenderedElement = (f, renderedElement) =>
      switch renderedElement {
      | IFlat(l) =>
        let nextL = ListTR.map(f, l);
        let unchanged = List.for_all2((===), l, nextL);
        unchanged ? renderedElement : IFlat(nextL);
      | INested(s, l) =>
        let nextL = ListTR.map(mapRenderedElement(f), l);
        let unchanged = List.for_all2((===), l, nextL);
        unchanged ? renderedElement : INested(s, nextL);
      };
    module OpaqueInstanceHash = {
      let addOpaqueInstances = (idTable, opaqueInstances) => {
        let add = opaqueInstance => {
          let Instance({component: {key}}) = opaqueInstance;
          key == Key.none ? () : Hashtbl.add(idTable, key, opaqueInstance);
        };
        List.iter(add, opaqueInstances);
      };
      let createPositionTable = opaqueInstances => {
        let posTable = Hashtbl.create(1);
        let add = (i, opaqueInstance) =>
          Hashtbl.add(posTable, i, opaqueInstance);
        List.iteri(add, opaqueInstances);
        posTable;
      };
      let addRenderedElement = (idTable, renderedElement) => {
        let rec aux =
          fun
          | IFlat(l) => addOpaqueInstances(idTable, l)
          | INested(_, l) => List.iter(aux, l);
        aux(renderedElement);
      };
      let createKeyTable = renderedElement =>
        lazy {
          let keyTable = Hashtbl.create(1);
          addRenderedElement(keyTable, renderedElement);
          keyTable;
        };
      let lookupKey = (table, key) => {
        let keyTable = Lazy.force(table);
        try (Some(Hashtbl.find(keyTable, key))) {
        | Not_found => None
        };
      };
      let lookupPosition = (posTable, i) =>
        try (Some(Hashtbl.find(posTable, i))) {
        | Not_found => None
        };
    };
    let getOpaqueInstance = (~useKeyTable, Element({key})) =>
      switch useKeyTable {
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
        'state 'action 'elementType .
        instance('state, 'action, 'elementType) =>
        instance('state, 'action, 'elementType)

    };

    /***
     * Initial render of an Element. Recurses to produce the entire tree of
     * instances.
     */
    let rec renderElement =
            (~useKeyTable=?, Element(component) as element)
            : opaqueInstance =>
      switch (getOpaqueInstance(useKeyTable, element)) {
      | Some(opaqueInstance) =>
        /* Throwaway update log: this is a render so no need to keep an update log. */
        let updateLog = UpdateLog.create();
        update(~updateLog, opaqueInstance, element);
      | None =>
        let (self, pendingStateUpdates, id) = createInitialSelf(~component);
        if (component.didMount !== defaultDidMount) {
          component.didMount(self);
        };
        let subElements = component.render(self);
        let instanceSubTree =
          switch component.elementType {
          | React => renderReactElement(subElements: reactElement)
          | Host => renderReactElement(subElements.children)
          };
        Instance({
          id,
          iState: self.state,
          component,
          subElements,
          instanceSubTree,
          pendingStateUpdates,
          element
        });
      }
    and renderReactElement = (~useKeyTable=?, reactElement) : renderedElement =>
      mapReactElement(renderElement(~useKeyTable?), reactElement)
    /***
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
          ~updateLog,
          originalOpaqueInstance,
          Element(nextComponent) as nextElement
        )
        : opaqueInstance => {
      let Instance(originalInstance) = originalOpaqueInstance;
      let updatedInstance =
        switch updateInstanceState {
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
        updatedComponent.handedOffInstance :=
          Some((originalInstance, updatedInstance));
        switch nextComponent.handedOffInstance^ {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some((originalInstance, updatedInstance)) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          updatedComponent.handedOffInstance := None;
          let updatedInstanceWithNewElement = {
            ...updatedInstance,
            component: nextComponent,
            element: nextElement
          };
          let oldSelf = createSelf(~instance=updatedInstance);
          let newState = nextComponent.willReceiveProps(oldSelf);
          /* We need to split up the check for state changes in two parts.
             The first part covers pending updates.
             The second part covers willReceiveProps. */
          let stateChanged =
            ! stateNotUpdated || newState !== updatedInstance.iState;
          let updatedInstanceWithNewState = {
            ...updatedInstanceWithNewElement,
            iState: newState
          };
          /* TODO: Invoke any lifecycles necessary. */
          let newSelf = createSelf(~instance=updatedInstance);
          if (nextComponent.shouldUpdate === defaultShouldUpdate
              || nextComponent.shouldUpdate({oldSelf, newSelf})) {
            if (nextComponent.willUpdate !== defaultWillUpdate) {
              nextComponent.willUpdate({oldSelf, newSelf});
            };
            let nextSubElements = nextComponent.render(newSelf);
            let {subElements, instanceSubTree} = updatedInstanceWithNewState;
            let (subTreeChange, nextInstanceSubTree) =
              updateRenderedElement(
                ~updateInstanceState?,
                ~updateLog,
                switch nextComponent.elementType {
                | React => (
                    instanceSubTree,
                    subElements: reactElement,
                    nextSubElements: reactElement
                  )
                | Host => (
                    instanceSubTree,
                    subElements.children,
                    nextSubElements.children
                  )
                }
              );
            switch (stateChanged, subTreeChange) {
            | (false, `NoChange) => originalOpaqueInstance
            | (stateChanged, subTreeChanged) =>
              if (nextComponent.didUpdate !== defaultDidUpdate) {
                let newSelf = createSelf(~instance=updatedInstance);
                nextComponent.didUpdate({oldSelf, newSelf});
              };
              let updatedInstanceWithNewSubtree = {
                ...updatedInstanceWithNewState,
                instanceSubTree: nextInstanceSubTree,
                subElements: nextSubElements
              };
              UpdateLog.addUpdateInstance(
                ~updateLog,
                ~stateChanged,
                ~subTreeChanged,
                originalInstance,
                updatedInstanceWithNewSubtree
              );
              Instance(updatedInstanceWithNewSubtree);
            };
          } else if (stateChanged) {
            UpdateLog.addUpdateInstance(
              ~updateLog,
              ~stateChanged,
              ~subTreeChanged=`NoChange,
              originalInstance,
              updatedInstanceWithNewState
            );
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
          updatedComponent.handedOffInstance := None;
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          let (self, pendingStateUpdates, id) =
            createInitialSelf(~component=nextComponent);
          let nextSubElements = nextComponent.render(self);
          let (nextSubtree, newOpaqueInstance) = {
            let nextSubtree =
              switch nextComponent.elementType {
              | React => renderReactElement(nextSubElements)
              | Host => renderReactElement(nextSubElements.children)
              };
            (
              nextSubtree,
              Instance({
                id,
                iState: self.state,
                component: nextComponent,
                subElements: nextSubElements,
                instanceSubTree: nextSubtree,
                pendingStateUpdates,
                element: nextElement
              })
            );
          };
          /*
           * ** Switching component type **
           * TODO: Invoke destruction lifecycle on previous component.
           * TODO: Invoke creation lifecycle on next component.
           */
          UpdateLog.addChangeComponent(
            ~updateLog,
            originalOpaqueInstance,
            newOpaqueInstance
          );
          newOpaqueInstance;
        };
      };
    }
    and updateRenderedElement =
        (
          ~updateInstanceState=?,
          ~updateLog,
          ~useKeyTable=?,
          (oldRenderedElement, oldReactElement, nextReactElement)
        ) => {
      /*
       * Key Policy for reactElement.
       * Nested elements determine shape: if the shape is not identical, re-render.
       * Flat elements use a positional match by default, where components at
       * the same position (from left) are matched for updates.
       * If the component has an explicit key, match the instance with the same key.
       * Note: components are matched for key across the entire reactElement structure.
       */
      let processElement =
          (~keyTable, ~posTable, position, Element(component) as element)
          : ([ | `NoChangeOrNested | `NewElement], opaqueInstance) =>
        if (component.key !== Key.none) {
          switch (OpaqueInstanceHash.lookupKey(keyTable, component.key)) {
          | Some(subOpaqueInstance) =>
            /* instance tree with the same component key */
            (
              `NoChangeOrNested,
              update(
                ~updateInstanceState?,
                ~updateLog,
                subOpaqueInstance,
                element
              )
            )
          | None =>
            /* not found: render a new instance */
            (`NewElement, renderElement(element))
          };
        } else {
          switch (OpaqueInstanceHash.lookupPosition(posTable, position)) {
          | Some(subOpaqueInstance) =>
            /* instance tree at the corresponding position */
            (
              `NoChangeOrNested,
              update(
                ~updateInstanceState?,
                ~updateLog,
                subOpaqueInstance,
                element
              )
            )
          | None =>
            /* not found: render a new instance */
            (`NewElement, renderElement(element))
          };
        };
      switch (oldRenderedElement, oldReactElement, nextReactElement) {
      | (
          INested(iName, instanceSubTrees),
          Nested(_, oldReactElements),
          Nested(_, [nextReactElement, ...nextReactElements])
        )
          when
            nextReactElements === oldReactElements && GlobalState.useTailHack^ =>
        /* Detected that nextReactElement was obtained by adding one element  */
        let addedElement = renderReactElement(nextReactElement);
        /* Prepend element*/
        (
          `PrependElement(addedElement),
          INested(iName, [addedElement, ...instanceSubTrees])
        );
      | (
          INested(oldName, oldRenderedElements),
          Nested(_, oldReactElements),
          Nested(_, nextReactElements)
        )
          when
            List.length(nextReactElements) === List.length(oldRenderedElements)
            && List.length(nextReactElements) === List.length(oldReactElements) =>
        let keyTable =
          switch useKeyTable {
          | None => OpaqueInstanceHash.createKeyTable(oldRenderedElement)
          | Some(keyTable) => keyTable
          };
        let newRenderedElementsAndUpdates =
          ListTR.map3(
            updateRenderedElement(
              ~updateInstanceState?,
              ~updateLog,
              ~useKeyTable=keyTable
            ),
            oldRenderedElements,
            oldReactElements,
            nextReactElements
          );
        let change =
          List.fold_left(
            (acc, (typ, _element)) =>
              switch (acc, typ) {
              | (acc, `NoChange) => acc
              | (`NoChange, x) => x
              | (acc, `Nested) => acc
              | (_, x) => x
              },
            `NoChange,
            newRenderedElementsAndUpdates
          );
        let newRenderedElement =
          INested(
            oldName,
            List.map(((_, x)) => x, newRenderedElementsAndUpdates)
          );
        switch change {
        | `NoChange => (`NoChange, oldRenderedElement)
        | `Nested => (`Nested, newRenderedElement)
        | `PrependElement(_)
        | `ReplaceElements(_, _) => (
            `ReplaceElements((oldRenderedElement, newRenderedElement)),
            newRenderedElement
          )
        };
      | (IFlat(oldOpaqueInstances), Flat(_), Flat(_)) =>
        let keyTable =
          switch useKeyTable {
          | None =>
            OpaqueInstanceHash.createKeyTable(IFlat(oldOpaqueInstances))
          | Some(keyTable) => keyTable
          };
        let posTable =
          OpaqueInstanceHash.createPositionTable(oldOpaqueInstances);
        /* Why shouldn't this be reached if there are different lengths?*/
        let newOpaqueInstancesAndUpdates =
          List.mapi(
            processElement(~keyTable, ~posTable),
            flattenReactElement(nextReactElement)
          );
        let replaced =
          List.fold_left(
            (acc, (elem, _)) =>
              switch elem {
              | `NoChangeOrNested => acc
              | `NewElement => true
              },
            false,
            newOpaqueInstancesAndUpdates
          );
        let newOpaqueInstances = List.map(snd, newOpaqueInstancesAndUpdates);
        if (replaced) {
          let newRenderedElement = IFlat(newOpaqueInstances);
          (
            `ReplaceElements((oldRenderedElement, newRenderedElement)),
            newRenderedElement
          );
        } else {
          let changed =
            List.exists2((!==), oldOpaqueInstances, newOpaqueInstances);
          if (changed) {
            (`Nested, IFlat(newOpaqueInstances));
          } else {
            (`NoChange, oldRenderedElement);
          };
        };
      | (_, _, _) =>
        let keyTable =
          switch useKeyTable {
          | None => OpaqueInstanceHash.createKeyTable(oldRenderedElement)
          | Some(keyTable) => keyTable
          };
        let newRenderedElement =
          renderReactElement(~useKeyTable=keyTable, nextReactElement);
        /* It's completely new, replaceElement */
        (
          `ReplaceElements((oldRenderedElement, newRenderedElement)),
          newRenderedElement
        );
      };
    };

    /***
     * Execute the pending updates at the top level of an instance tree.
     * If no state change is performed, the argument is returned unchanged.
     */
    let executePendingStateUpdates:
      'state 'action 'elementType .
      instance('state, 'action, 'elementType) =>
      instance('state, 'action, 'elementType)
     =
      instance => {
        let executeUpdate = (~state, stateUpdate) =>
          switch (stateUpdate(state)) {
          | NoUpdate => state
          | Update(newState) => newState
          };
        let rec executeUpdates = (~state, stateUpdates) =>
          switch stateUpdates {
          | [] => state
          | [stateUpdate, ...otherStateUpdates] =>
            let nextState = executeUpdate(~state, stateUpdate);
            executeUpdates(~state=nextState, otherStateUpdates);
          };
        let pendingUpdates = List.rev(instance.pendingStateUpdates^);
        instance.pendingStateUpdates := [];
        let nextState = executeUpdates(~state=instance.iState, pendingUpdates);
        instance.iState === nextState ?
          instance : {...instance, iState: nextState};
      };

    /***
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = (~updateLog, opaqueInstance) => {
      let Instance({element}) = opaqueInstance;
      update(
        ~updateLog,
        ~updateInstanceState={f: executePendingStateUpdates},
        opaqueInstance,
        element
      );
    };
  };
  module RenderedElement = {

    /***
     * Rendering produces a list of instance trees.
     */
    type t = renderedElement;
    type subtreeChange = [
      | `Nested
      | `PrependElement(renderedElement)
      | `ReplaceElements(renderedElement, renderedElement)
    ];
    type topLevelUpdate = {
      subtreeChange,
      updateLog: UpdateLog.t
    };
    let listToRenderedElement = renderedElements =>
      INested("List", renderedElements);
    let render = reactElement : t => Render.renderReactElement(reactElement);
    let update =
        (renderedElement: t, reactElement)
        : (t, option(topLevelUpdate)) => {
      let updateLog = UpdateLog.create();
      let (topLevelChange, newRenderedElement) =
        Render.updateRenderedElement(
          ~updateLog,
          (renderedElement, reactElement, reactElement)
        );
      switch topLevelChange {
      | `NoChange => (renderedElement, None)
      | `Nested as subtreeChange
      | `PrependElement(_) as subtreeChange
      | `ReplaceElements(_, _) as subtreeChange => (
          newRenderedElement,
          Some({subtreeChange, updateLog})
        )
      };
    };

    /***
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = (renderedElement: t) : (t, UpdateLog.t) => {
      let updateLog = UpdateLog.create();
      let newRenderedElement =
        Render.mapRenderedElement(
          Render.flushPendingUpdates(~updateLog),
          renderedElement
        );
      (newRenderedElement, updateLog);
    };
  };
  let statelessComponent:
    (~useDynamicKey: bool=?, string) =>
    component(stateless, actionless, reactElement) =
    (~useDynamicKey=?, debugName) => {
      ...basicComponent(~useDynamicKey?, debugName, React),
      initialState: () => ()
    };
  let statefulComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec('state, stateless, actionless, reactElement) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let reducerComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec('state, stateless, 'action, reactElement) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, React);
  let statelessNativeComponent:
    (~useDynamicKey: bool=?, string) =>
    component(stateless, actionless, nativeElement) =
    (~useDynamicKey=?, debugName) => {
      ...basicComponent(~useDynamicKey?, debugName, Host),
      initialState: () => ()
    };
  let statefulNativeComponent:
    (~useDynamicKey: bool=?, string) =>
    componentSpec('state, stateless, actionless, nativeElement) =
    (~useDynamicKey=?, debugName) =>
      basicComponent(~useDynamicKey?, debugName, Host);
  let element = (~key as argumentKey=Key.none, component) => {
    let key =
      argumentKey != Key.none ?
        argumentKey : component.key == Key.none ? Key.none : Key.create();
    let componentWithKey = key == Key.none ? component : {...component, key};
    Flat([Element(componentWithKey)]);
  };
  let arrayToElement = (a: array(reactElement)) : reactElement =>
    Nested("Array", Array.to_list(a));
  let listToElement = l => Nested("List", l);
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
