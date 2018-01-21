module type HostImplementation = {
  type hostView;
  let getInstance: int => option(hostView);
};

module Make = (Implementation: HostImplementation) => {
  module GlobalState = {
    let debug = ref(true);
    let componentKeyCounter = ref(0);
    let instanceIdCounter = ref(0);
    let reset = () => {
      debug := true;
      componentKeyCounter := 0;
      instanceIdCounter := 1 /* id 0 reserved for defaultStringInstance */
    };
    /*
     * Use physical equality to recognize that an element was added to the list of children.
     * Note: this currently does not check for pending updates on components in the list.
     */
    let useTailHack = ref(false);
  };
  module Key = {
    type t = int;
    let none = (-1);
    let first = 0;
    let create = () => {
      incr(GlobalState.componentKeyCounter);
      GlobalState.componentKeyCounter^
    };
  };
  type sideEffects = unit => unit;
  type stateless = unit;
  type actionless = unit;
  module Callback = {
    type t('payload) = 'payload => unit;
    let default = (_event) => ();
    let chain = (handlerOne, handlerTwo, payload) => {
      handlerOne(payload);
      handlerTwo(payload)
    };
  };
  module Node = {
    type context =
      | View(Implementation.hostView)
      | Container;
    let nullContext = Container;
  };
  module Layout = {
    include Layout.Create(Node, FloatEncoding);
    include LayoutSupport.LayoutTypes;
    type t = LayoutSupport.LayoutTypes.node;
    let defaultStyle = LayoutSupport.defaultStyle;
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
    make: int => Implementation.hostView,
    setProps: Implementation.hostView => unit,
    children: reactElement,
    style: Layout.LayoutSupport.LayoutTypes.cssStyle
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
    handedOffInstance: ref(option(instance('state, 'action, 'element))) /* Used to avoid Obj.magic in update */,
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
    subElements: reactElement,
    /***
     * List of state updates pending for this instance.
     * Stored in reverse order.
     */
    pendingStateUpdates: ref(list(('state => update('state, 'action)))),
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
    | Instance(instance('state, 'action, reactElement)): opaqueInstance
    | NativeInstance(nativeElement, instance('state, 'action, nativeElement)): opaqueInstance;
  let logString = (txt) =>
    GlobalState.debug^ ?
      {
        print_string(txt);
        print_newline()
      } :
      ();
  let defaultShouldUpdate = (_oldNewSef) => true;
  let defaultWillUpdate = (_oldNewSef) => ();
  let defaultDidUpdate = (_oldNewSef) => ();
  let defaultDidMount = (_self) => ();
  let basicComponent = (~useDynamicKey=false, debugName, elementType) => {
    let key = useDynamicKey ? Key.first : Key.none;
    {
      debugName,
      elementType,
      willReceiveProps: ({state}) => state,
      didMount: defaultDidMount,
      didUpdate: defaultDidUpdate,
      willUnmount: (_self) => (),
      willUpdate: defaultWillUpdate,
      shouldUpdate: defaultShouldUpdate,
      render: (_self) => assert false,
      initialState: () => (),
      reducer: (_action, _state) => NoUpdate,
      printState: (_state) => "",
      handedOffInstance: ref(None),
      key
    }
  };

  /*** Tail-recursive functions on lists */
  module ListTR = {
    let useTailRecursion = (l) =>
      switch l {
      | [_, _, _, _, _, _, _, _, _, _, ..._] => true
      | _ => false
      };
    let concat = (list) => {
      let rec aux = (acc, l) =>
        switch l {
        | [] => List.rev(acc)
        | [x, ...rest] => aux(List.rev_append(x, acc), rest)
        };
      useTailRecursion(list) ? aux([], list) : List.concat(list)
    };
    let map = (f, list) =>
      useTailRecursion(list) ?
        List.rev_map(f, List.rev(list)) : List.map(f, list);
    let rev_map3 = (f, list1, list2, list3) => {
      let rec aux = (acc) =>
        fun
        | ([], [], []) => acc
        | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) =>
          aux([f((x1, x2, x3)), ...acc], (nextList1, nextList2, nextList3))
        | _ => assert false;
      aux([], (list1, list2, list3))
    };
    let map3 = (f, list1, list2, list3) =>
      rev_map3(f, List.rev(list1), List.rev(list2), List.rev(list3));
  };

  /*** Log of operations performed to update an instance tree. */
  module UpdateLog = {
    type update = {
      oldId: int,
      newId: int,
      oldOpaqueInstance: opaqueInstance,
      newOpaqueInstance: opaqueInstance,
      componentChanged: bool,
      stateChanged: bool,
      subTreeChanged: bool
    };
    type entry =
      | UpdateInstance(update)
      | NewRenderedElement(renderedElement);
    type t = ref(list(entry));
    let create = () => ref([]);
    let add = (updateLog, x) => updateLog := [x, ...updateLog^];
  };
  module Render = {
    let createSelf = (~instance) : self(_) => {
      state: instance.iState,
      reduce: (payloadToAction, payload) => {
        logString("Calling reduce on " ++ instance.component.debugName);
        let action = payloadToAction(payload);
        let stateUpdate = instance.component.reducer(action);
        instance.pendingStateUpdates :=
          [stateUpdate, ...instance.pendingStateUpdates^]
      },
      act: (action) => {
        logString("Calling act on " ++ instance.component.debugName);
        let stateUpdate = instance.component.reducer(action);
        instance.pendingStateUpdates :=
          [stateUpdate, ...instance.pendingStateUpdates^]
      }
    };
    let createInstance = (~component, ~element, ~instanceSubTree, ~subElements) => {
      let iState = component.initialState();
      let id = GlobalState.instanceIdCounter^;
      incr(GlobalState.instanceIdCounter);
      {
        component,
        element,
        iState,
        instanceSubTree,
        subElements,
        pendingStateUpdates: ref([]),
        id
      }
    };
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
        unchanged ? renderedElement : IFlat(nextL)
      | INested(s, l) =>
        let nextL = ListTR.map(mapRenderedElement(f), l);
        let unchanged = List.for_all2((===), l, nextL);
        unchanged ? renderedElement : INested(s, nextL)
      };
    module OpaqueInstanceHash = {
      let addOpaqueInstances = (idTable, opaqueInstances) => {
        let add = (opaqueInstance) => {
          let NativeInstance(_, {component: {key}}) |
              Instance({component: {key}}) = opaqueInstance;
          key == Key.none ? () : Hashtbl.add(idTable, key, opaqueInstance)
        };
        List.iter(add, opaqueInstances)
      };
      let createPositionTable = (opaqueInstances) => {
        let posTable = Hashtbl.create(1);
        let add = (i, opaqueInstance) =>
          Hashtbl.add(posTable, i, opaqueInstance);
        List.iteri(add, opaqueInstances);
        posTable
      };
      let addRenderedElement = (idTable, renderedElement) => {
        let rec aux =
          fun
          | IFlat(l) => addOpaqueInstances(idTable, l)
          | INested(_, l) => List.iter(aux, l);
        aux(renderedElement)
      };
      let createKeyTable = (renderedElement) =>
        lazy {
          let keyTable = Hashtbl.create(1);
          addRenderedElement(keyTable, renderedElement);
          keyTable
        };
      let lookupKey = (table, key) => {
        let keyTable = Lazy.force(table);
        try (Some(Hashtbl.find(keyTable, key))) {
        | Not_found => None
        }
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
        update(~updateLog, opaqueInstance, element)
      | None =>
        let instance =
          createInstance(
            ~component,
            ~element,
            ~instanceSubTree=IFlat([]),
            ~subElements=Flat([])
          );
        let self = createSelf(~instance);
        if (component.didMount !== defaultDidMount) {
          component.didMount(self)
        };
        switch component.elementType {
        | React =>
          let reactElement: reactElement = component.render(self);
          let instanceSubTree = renderReactElement(reactElement);
          Instance({...instance, instanceSubTree, subElements: reactElement})
        | Host =>
          let reactElement = component.render(self);
          let instanceSubTree = renderReactElement(reactElement.children);
          NativeInstance(
            reactElement,
            {...instance, instanceSubTree, subElements: reactElement.children}
          )
        }
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
     */
    and update =
        (
          ~updateOpaqueInstanceState=?,
          ~updateLog,
          opaqueInstance,
          Element(nextComponent) as nextElement
        )
        : opaqueInstance => {
      let updatedOpaqueInstance =
        switch updateOpaqueInstanceState {
        | Some(f) => f(opaqueInstance)
        | None => opaqueInstance
        };
      let stateNotUpdated = opaqueInstance === updatedOpaqueInstance;
      let bailOut = {
        let NativeInstance(_, {element}) | Instance({element}) = opaqueInstance;
        stateNotUpdated && element === nextElement
      };
      let logUpdate =
          (
            ~componentChanged,
            ~stateChanged,
            ~subTreeChanged,
            newOpaqueInstance
          ) => {
        let NativeInstance(_, {id}) | Instance({id}) = opaqueInstance;
        let NativeInstance(_, {id: newId}) | Instance({id: newId}) = newOpaqueInstance;
        let comp = componentChanged ? " component" : "";
        let st = stateChanged ? " state" : "";
        let sub = subTreeChanged ? " subtree" : "";
        UpdateLog.add(
          updateLog,
          UpdateLog.UpdateInstance({
            oldId: id,
            newId,
            oldOpaqueInstance: opaqueInstance,
            newOpaqueInstance,
            componentChanged,
            stateChanged,
            subTreeChanged
          })
        );
        logString(
          Printf.sprintf(
            "instance #%d updated to instance #%d changed:%s%s%s",
            id,
            newId,
            comp,
            st,
            sub
          )
        )
      };
      if (bailOut) {
        opaqueInstance
      } else {
        let resetHandedOffInstance =
          switch updatedOpaqueInstance {
          | NativeInstance(_, {component} as updatedInstance) =>
            component.handedOffInstance := Some(updatedInstance);
            (() => component.handedOffInstance := None)
          | Instance({component} as updatedInstance) =>
            component.handedOffInstance := Some(updatedInstance);
            (() => component.handedOffInstance := None)
          };
        switch nextComponent.handedOffInstance^ {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some(updatedInstance) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          resetHandedOffInstance();
          logString("Updating " ++ nextComponent.debugName);
          let instance = {
            ...updatedInstance,
            component: nextComponent,
            element: nextElement
          };
          let oldSelf = createSelf(~instance);
          let newState = nextComponent.willReceiveProps(oldSelf);
          let newInstance = {...instance, iState: newState};
          let stateChanged =
            /* We need to split up the check for state changes in two parts.
               The first part covers pending updates.
               The second part covers changes during case A processing. */
            ! stateNotUpdated || newState !== updatedInstance.iState;
          let newSelf = createSelf(~instance=newInstance);
          if (nextComponent.shouldUpdate === defaultShouldUpdate
              || nextComponent.shouldUpdate({oldSelf, newSelf})) {
            if (nextComponent.willUpdate !== defaultWillUpdate) {
              nextComponent.willUpdate({oldSelf, newSelf})
            };
            let nextSubElements = nextComponent.render(newSelf);
            /* TODO: Invoke any lifecycles necessary. */
            let Instance({subElements, instanceSubTree}) |
                NativeInstance(_, {subElements, instanceSubTree}) = updatedOpaqueInstance;
            let (nextInstanceSubTree, newNewInstance, newOpaqueInstance) =
              switch nextComponent.elementType {
              | Host =>
                let nextInstanceSubTree =
                  updateRenderedElement(
                    ~updateOpaqueInstanceState?,
                    ~updateLog,
                    (instanceSubTree, subElements, nextSubElements.children)
                  );
                let newNewInstance = {
                  ...newInstance,
                  instanceSubTree: nextInstanceSubTree,
                  subElements: nextSubElements.children
                };
                (
                  nextInstanceSubTree,
                  newNewInstance,
                  NativeInstance(nextSubElements, instance)
                )
              | React =>
                let nextInstanceSubTree =
                  updateRenderedElement(
                    ~updateOpaqueInstanceState?,
                    ~updateLog,
                    (
                      instanceSubTree,
                      subElements,
                      nextSubElements: reactElement
                    )
                  );
                let newNewInstance = {
                  ...newInstance,
                  instanceSubTree: nextInstanceSubTree,
                  subElements: nextSubElements
                };
                (nextInstanceSubTree, newNewInstance, Instance(instance))
              };
            if (nextComponent.didUpdate !== defaultDidUpdate) {
              let newNewSelf = createSelf(~instance=newNewInstance);
              nextComponent.didUpdate({oldSelf, newSelf: newNewSelf})
            };
            let subTreeChanged = {
              let NativeInstance(_, {instanceSubTree}) |
                  Instance({instanceSubTree}) = opaqueInstance;
              nextInstanceSubTree !== instanceSubTree
            };
            logUpdate(
              ~componentChanged=false,
              ~stateChanged,
              ~subTreeChanged,
              newOpaqueInstance
            );
            newOpaqueInstance
          } else {
            switch newInstance.component.elementType {
            | React =>
              let newOpaqueInstance = Instance(newInstance);
              logUpdate(
                ~componentChanged=false,
                ~stateChanged,
                ~subTreeChanged=false,
                newOpaqueInstance
              );
              newOpaqueInstance
            | Host =>
              let nativeElement =
                switch updatedOpaqueInstance {
                | NativeInstance(nativeElement, _) => nativeElement
                | _ =>
                  /* This cannot happen since they have the same type*/
                  assert false
                };
              let newOpaqueInstance =
                NativeInstance(nativeElement, newInstance);
              logUpdate(
                ~componentChanged=false,
                ~stateChanged,
                ~subTreeChanged=false,
                newOpaqueInstance
              );
              newOpaqueInstance
            }
          }
        /*
         * Case B: The next element is *not* of the same component class. We know
         * because otherwise we would have observed the mutation on
         * `nextComponentClass`.
         */
        | None =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          resetHandedOffInstance();
          let NativeInstance(
                _,
                {component: {debugName}, instanceSubTree, subElements}
              ) |
              Instance({component: {debugName}, instanceSubTree, subElements}) = updatedOpaqueInstance;
          let nextInstance =
            createInstance(
              ~component=nextComponent,
              ~element=nextElement,
              ~instanceSubTree,
              ~subElements
            );
          let self = createSelf(~instance=nextInstance);
          let nextSubElements = nextComponent.render(self);
          logString(
            Printf.sprintf(
              "Switching Component Types from: %s to %s",
              debugName,
              nextComponent.debugName
            )
          );
          /* TODO: Invoke destruction lifecycle on previous component. */
          /* TODO: Invoke creation lifecycle on next component. */
          let newOpaqueInstance =
            switch nextComponent.elementType {
            | React =>
              let nextSubtree = renderReactElement(nextSubElements);
              Instance({
                ...nextInstance,
                instanceSubTree: nextSubtree,
                subElements: nextSubElements
              })
            | Host =>
              let nextSubtree = renderReactElement(nextSubElements.children);
              NativeInstance(
                nextSubElements,
                {
                  ...nextInstance,
                  instanceSubTree: nextSubtree,
                  subElements: nextSubElements.children
                }
              )
            };
          logUpdate(
            ~componentChanged=true,
            ~stateChanged=true,
            ~subTreeChanged=true,
            newOpaqueInstance
          );
          newOpaqueInstance
        }
      }
    }
    and updateRenderedElement =
        (
          ~updateOpaqueInstanceState=?,
          ~updateLog,
          ~useKeyTable=?,
          ~topLevelUpdate=false,
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
          : opaqueInstance =>
        if (component.key !== Key.none) {
          switch (OpaqueInstanceHash.lookupKey(keyTable, component.key)) {
          | Some(subOpaqueInstance) =>
            /* instance tree with the same component id */
            update(
              ~updateOpaqueInstanceState?,
              ~updateLog,
              subOpaqueInstance,
              element
            )
          | None =>
            /* not found: render a new instance */
            renderElement(element)
          }
        } else {
          switch (OpaqueInstanceHash.lookupPosition(posTable, position)) {
          | Some(subOpaqueInstance) =>
            /* instance tree at the corresponding position */
            update(
              ~updateOpaqueInstanceState?,
              ~updateLog,
              subOpaqueInstance,
              element
            )
          | None =>
            /* not found: render a new instance */
            renderElement(element)
          }
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
        INested(
          iName,
          [renderReactElement(nextReactElement), ...instanceSubTrees]
        )
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
        let newRenderedElements =
          ListTR.map3(
            updateRenderedElement(
              ~updateOpaqueInstanceState?,
              ~updateLog,
              ~useKeyTable=keyTable
            ),
            oldRenderedElements,
            oldReactElements,
            nextReactElements
          );
        let changed =
          List.exists2((!==), oldRenderedElements, newRenderedElements);
        let newRenderedElement =
          changed ? INested(oldName, newRenderedElements) : oldRenderedElement;
        if (topLevelUpdate && changed) {
          UpdateLog.add(updateLog, NewRenderedElement(newRenderedElement))
        };
        newRenderedElement
      | (IFlat(oldOpaqueInstances), Flat(_), Flat(_)) =>
        let keyTable =
          switch useKeyTable {
          | None =>
            OpaqueInstanceHash.createKeyTable(IFlat(oldOpaqueInstances))
          | Some(keyTable) => keyTable
          };
        let posTable =
          OpaqueInstanceHash.createPositionTable(oldOpaqueInstances);
        let newOpaqueInstances =
          List.mapi(
            processElement(~keyTable, ~posTable),
            flattenReactElement(nextReactElement)
          );
        let changed =
          List.exists2((!==), oldOpaqueInstances, newOpaqueInstances);
        let newRenderedElement =
          changed ? IFlat(newOpaqueInstances) : oldRenderedElement;
        if (topLevelUpdate && changed) {
          UpdateLog.add(updateLog, NewRenderedElement(newRenderedElement))
        };
        newRenderedElement
      | (_, _, _) =>
        let keyTable =
          switch useKeyTable {
          | None => OpaqueInstanceHash.createKeyTable(oldRenderedElement)
          | Some(keyTable) => keyTable
          };
        let newRenderedElement =
          renderReactElement(~useKeyTable=keyTable, nextReactElement);
        if (topLevelUpdate) {
          UpdateLog.add(updateLog, NewRenderedElement(newRenderedElement))
        };
        newRenderedElement
      }
    };

    /***
     * Execute the pending updates at the top level of an instance tree.
     * If no state change is performed, the argument is returned unchanged.
     */
    let executePendingStateUpdates = (opaqueInstance) => {
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
          executeUpdates(~state=nextState, otherStateUpdates)
        };
      switch opaqueInstance {
      | NativeInstance(elem, instance) =>
        let pendingUpdates = List.rev(instance.pendingStateUpdates^);
        instance.pendingStateUpdates := [];
        let nextState = executeUpdates(~state=instance.iState, pendingUpdates);
        instance.iState === nextState ?
          opaqueInstance :
          NativeInstance(elem, {...instance, iState: nextState})
      | Instance(instance) =>
        let pendingUpdates = List.rev(instance.pendingStateUpdates^);
        instance.pendingStateUpdates := [];
        let nextState = executeUpdates(~state=instance.iState, pendingUpdates);
        instance.iState === nextState ?
          opaqueInstance : Instance({...instance, iState: nextState})
      }
    };

    /***
     * Flush the pending updates in an instance tree.
     * TODO: invoke lifecycles
     */
    let flushPendingUpdates = (~updateLog, opaqueInstance) => {
      let NativeInstance(_, {element}) | Instance({element}) = opaqueInstance;
      update(
        ~updateLog,
        ~updateOpaqueInstanceState=executePendingStateUpdates,
        opaqueInstance,
        element
      )
    };
  };
  module RenderedElement = {

    /***
     * Rendering produces a list of instance trees.
     */
    type t = renderedElement;
    let listToRenderedElement = (renderedElements) =>
      INested("List", renderedElements);
    let render = (reactElement) : t => Render.renderReactElement(reactElement);
    let update = (renderedElement: t, reactElement) : (t, UpdateLog.t) => {
      let updateLog = UpdateLog.create();
      let newRenderedElement =
        Render.updateRenderedElement(
          ~topLevelUpdate=true,
          ~updateLog,
          (renderedElement, reactElement, reactElement)
        );
      (newRenderedElement, updateLog)
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
      (newRenderedElement, updateLog)
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
  let element = (~key as argumentKey=Key.none, component) => {
    let key =
      argumentKey != Key.none ?
        argumentKey : component.key == Key.none ? Key.none : Key.create();
    let componentWithKey = key == Key.none ? component : {...component, key};
    Flat([Element(componentWithKey)])
  };
  let arrayToElement = (a: array(reactElement)) : reactElement =>
    Nested("Array", Array.to_list(a));
  let listToElement = (l) => Nested("List", l);
  /* TODO: don't make this public
   * Instead, wrap every nativeElement in a stateless/statefulComponent.
   * In willReceiveProps/didReceiveProps invoke the side effect (prop mutation)
   */
  module ReactDOMRe = {
    type reactDOMProps;
    let createElement =
        (name, ~props as _props=?, elementArray: array(reactElement))
        : reactElement =>
      Nested(name, Array.to_list(elementArray));
  };
  module OutputTree = {
    type tree =
      | Container(node)
      | Concrete(Implementation.hostView, nativeElement, node)
    and node = {
      mutable sub: list(tree),
      mutable id: int,
      mutable nearestParentView: Implementation.hostView
    };
    type sideEffect;
    type forest = list(tree);
    let rec unmountForest = (tree) =>
      switch tree {
      | [] => ()
      | l =>
        List.iter(
          (node) =>
            switch node {
            | Container(node) => unmountForest(node.sub)
            | Concrete(view, _, node) =>
              print_endline("Unmounting a concrete view")
            /*ignore(NativeView.removeChild(node.nearestParentView, view))*/
            },
          l
        )
      };
    let rec mountForest = (tree) =>
      switch tree {
      | [] => ()
      | l =>
        List.iter(
          (node) =>
            switch node {
            | Container(node) => mountForest(node.sub)
            | Concrete(view, nativeComponent, node) =>
              mountForest(node.sub);
              nativeComponent.setProps(view)
            /*ignore(NativeView.addChild(node.nearestParentView, view))*/
            },
          l
        )
      };
    type t = forest;
    let rec fromOpaqueInstance = (nearestParentView, opaqueInstance) : tree =>
      switch opaqueInstance {
      | NativeInstance(nativeElement, {id, instanceSubTree}) =>
        let view =
          switch (Implementation.getInstance(id)) {
          | Some(x) => x
          | None => nativeElement.make(id)
          };
        let subTreeInstances = Render.flattenRenderedElement(instanceSubTree);
        let sub = ListTR.map(fromOpaqueInstance(view), subTreeInstances);
        let node = {id, sub, nearestParentView};
        Concrete(view, nativeElement, node)
      | Instance({id, instanceSubTree}) =>
        let subTreeInstances = Render.flattenRenderedElement(instanceSubTree);
        let sub =
          ListTR.map(fromOpaqueInstance(nearestParentView), subTreeInstances);
        let node = {id, sub, nearestParentView};
        Container(node)
      };
    let fromRenderedElement = (nearestParentView, renderedElement) =>
      ListTR.map(
        fromOpaqueInstance(nearestParentView),
        Render.flattenRenderedElement(renderedElement)
      );
    let applyUpdateLog = (updateLog, forest, parent) => {
      open UpdateLog;
      let rec applyEntryTree = (t, entry) : option(tree) =>
        switch entry {
        | NewRenderedElement(_) => assert false
        | UpdateInstance({
            oldId,
            newId,
            oldOpaqueInstance,
            newOpaqueInstance,
            componentChanged,
            stateChanged,
            subTreeChanged
          }) =>
          switch t {
          | Concrete(_, _, n)
          | Container(n) =>
            if (n.id === oldId) {
              /* The same instance ? */
              if (oldId !== newId) {
                n.id =
                  newId
                  /*
                      Update ID
                   */
              };
              let NativeInstance(_, {instanceSubTree}) |
                  Instance({instanceSubTree}) = newOpaqueInstance;
              /*
               if (componentChanged) {
                 n.name = component.debugName
                 willUnmount
               };
               if (stateChanged) {
                 n.state = component.printState(iState)
               };
               */
              if (subTreeChanged) {
                let NativeInstance(_, {instanceSubTree: oldInstanceSubTree}) |
                    Instance({instanceSubTree: oldInstanceSubTree}) = oldOpaqueInstance;
                let sub =
                  switch (instanceSubTree, oldInstanceSubTree) {
                  | (
                      INested(
                        _,
                        [nextRenderedElement, ...nextRenderedElements]
                      ),
                      INested(_, oldRenderedElements)
                    )
                      when nextRenderedElements === oldRenderedElements =>
                    let headTrees =
                      fromRenderedElement(
                        n.nearestParentView,
                        nextRenderedElement
                      );
                    /* Add single headTrees */
                    mountForest(headTrees);
                    List.rev_append(List.rev(headTrees), n.sub)
                  | _ =>
                    let subTreeInstances =
                      Render.flattenRenderedElement(instanceSubTree);
                    /*Remove all that were here, mount new ones */
                    let forest =
                      ListTR.map(
                        fromOpaqueInstance(n.nearestParentView),
                        subTreeInstances
                      );
                    unmountForest(n.sub);
                    mountForest(forest);
                    forest
                  };
                n.sub = sub
              };
              Some(t)
            } else {
              switch (applyEntryForest(n.sub, entry)) {
              | None => None
              | Some(newSub) =>
                if (n.sub !== newSub) {
                  /* Remove all that were here and mount new ones */
                  unmountForest(n.sub);
                  n.sub = newSub
                };
                Some(t)
              }
            }
          }
        }
      and applyEntryForest = (f, entry) : option(forest) =>
        switch f {
        | [] => None
        | [t, ...nextF] =>
          switch (applyEntryTree(t, entry)) {
          | None =>
            switch (applyEntryForest(nextF, entry)) {
            | None => None
            | Some(newForest) => Some([t, ...newForest])
            }
          | Some(newT) => Some([newT, ...nextF])
          }
        };
      let applyEntryForestToplevel = (f, entry, parent) =>
        switch entry {
        | NewRenderedElement(renderedElement) =>
          /* Mount new rendered element */
          let forest = fromRenderedElement(parent, renderedElement);
          mountForest(forest);
          Some(forest)
        | UpdateInstance(_) => applyEntryForest(f, entry)
        };
      List.fold_left(
        (f, entry) =>
          switch (applyEntryForestToplevel(f, entry, parent)) {
          | None => f
          | Some(newF) => newF
          },
        forest,
        updateLog^
      )
    };
  };
  module LayoutTest = {
    open Layout;
    open OutputTree;
    let rec make = (tree) =>
      switch tree {
      | Container(node) =>
        LayoutSupport.createNode(
          ~withChildren=Array.of_list(List.map(make, node.sub)),
          ~andStyle=LayoutSupport.defaultStyle,
          Container
        )
      | Concrete(view, nativeComponent, node) =>
        LayoutSupport.createNode(
          ~withChildren=Array.of_list(List.map(make, node.sub)),
          ~andStyle=nativeComponent.style,
          View(view)
        )
      };
    let make = (~root, ~outputTree as forest, ~width, ~height) => {
      let children =
        List.fold_left((t, element) => [make(element), ...t], [], forest);
      LayoutSupport.createNode(
        ~withChildren=Array.of_list(children),
        ~andStyle={...LayoutSupport.defaultStyle, width, height},
        View(root)
      )
    };
    let performLayout = (root) => {
      Layout.layoutNode(
        root,
        FixedEncoding.cssUndefined,
        FixedEncoding.cssUndefined,
        Ltr
      );
      let rec traverseLayout = (node: Layout.LayoutSupport.LayoutTypes.node) => {
        Array.iter(traverseLayout, node.children);
        switch node.context {
        | Container => ()
        | View(view) =>
          /*
           ignore(
             NativeView.setFrame(
               node.layout.left,
               node.layout.top,
               node.layout.width,
               node.layout.height,
               view
             )
           )
           */
          ()
        }
      };
      traverseLayout(root)
    };
  };
  module RemoteAction = {
    type t('action) = {mutable act: 'action => unit};
    let actDefault = (_action) => ();
    let create = () => {act: actDefault};
    let subscribe = (~act, x) =>
      if (x.act === actDefault) {
        x.act = act
      };
    let act = (x, ~action) => x.act(action);
  };
};
