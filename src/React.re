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

module NativeView = {
  type t;
  external getWindow : unit => t = "View_getWindow";
  external makeInstance : int => t = "View_newView";
  external addChild : (t, t) => t = "View_addChild";
  external removeChild : (t, t) => t = "View_removeChild";
  external getInstance : int => option(t) = "View_getInstance";
  [@noalloc] external setFrame : (int, int, int, int, t) => t =
    "View_setFrame";
};

module Node = {
  type context =
    | View(NativeView.t)
    | Container;
  let nullContext = Container;
};

module Layout = {
  include Layout.Create(Node, FloatEncoding);
  include LayoutSupport.LayoutTypes;
  type t = LayoutSupport.LayoutTypes.node;
  let defaultStyle = LayoutSupport.defaultStyle;
};

type reduce('payload, 'action) = ('payload => 'action) => Callback.t('payload);

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
  | Element(component('state, 'action)): element
  | HostElement(nativeComponent, Layout.LayoutSupport.LayoutTypes.cssStyle): element
/***
 * We will want to replace this with a more efficient data structure.
 */
and reactElement =
  | Flat(list(element))
  | Nested(string, list(reactElement))
and nativeComponent = {
  name: string,
  make: int => NativeView.t,
  setProps: NativeView.t => unit,
  children: reactElement,
  nativeKey: int
}
and renderedElement =
  | IFlat(list(opaqueInstance))
  | INested(string, list(renderedElement))
and oldNewSelf('state, 'action) = {
  oldSelf: self('state, 'action),
  newSelf: self('state, 'action)
}
and componentSpec('state, 'initialState, 'action) = {
  debugName: string,
  willReceiveProps: self('state, 'action) => 'state,
  didMount: self('state, 'action) => unit,
  didUpdate: oldNewSelf('state, 'action) => unit,
  willUnmount: self('state, 'action) => unit /* TODO: currently unused */,
  willUpdate: oldNewSelf('state, 'action) => unit,
  shouldUpdate: oldNewSelf('state, 'action) => bool,
  render: self('state, 'action) => reactElement,
  initialState: unit => 'initialState,
  reducer: ('action, 'state) => update('state, 'action),
  printState: 'state => string /* for internal debugging */,
  handedOffInstance: ref(option(instance('state, 'action))) /* Used to avoid Obj.magic in update */,
  key: int
}
and component('state, 'action) = componentSpec('state, 'state, 'action)
/***
 * Elements turn into instances at the right time. These instances record the
 * most recent state among other things.
 */
and instance('state, 'action) = {
  component: component('state, 'action),
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
  | Instance(instance('state, 'action)): opaqueInstance;

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

let basicComponent =
    (~useDynamicKey=false, debugName)
    : componentSpec(_, stateless, _) => {
  let key = useDynamicKey ? Key.first : Key.none;
  {
    debugName,
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
  let defaultHostComponent = basicComponent("Host");
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
        let Instance({component: {key}}) = opaqueInstance;
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
  let getOpaqueInstance = (~useKeyTable, element) =>
    switch useKeyTable {
    | None => None
    | Some(keyTable) =>
      OpaqueInstanceHash.lookupKey(
        keyTable,
        switch element {
        | Element(component) => component.key
        | HostElement(component, _) => component.nativeKey
        }
      )
    };

  /***
   * Initial render of an Element. Recurses to produce the entire tree of
   * instances.
   */
  let rec renderElement = (~useKeyTable=?, element) : opaqueInstance =>
    switch (getOpaqueInstance(useKeyTable, element)) {
    | Some(opaqueInstance) =>
      /* Throwaway update log: this is a render so no need to keep an update log. */
      let updateLog = UpdateLog.create();
      update(~updateLog, opaqueInstance, element)
    | None =>
      switch element {
      | Element(component) =>
        let instance =
          createInstance(
            ~component,
            ~element,
            ~instanceSubTree=IFlat([]),
            ~subElements=Flat([])
          );
        let self = createSelf(~instance);
        let reactElement = component.render(self);
        if (component.didMount !== defaultDidMount) {
          component.didMount(self)
        };
        let length =
          switch reactElement {
          | Flat(elements) => List.length(elements)
          | Nested(_, reactElements) => List.length(reactElements)
          };
        logString(
          Printf.sprintf(
            "Creating instance #%d of %s with %d subelements",
            instance.id,
            component.debugName,
            length
          )
        );
        let instanceSubTree = renderReactElement(reactElement);
        Instance({...instance, instanceSubTree, subElements: reactElement})
      | HostElement({children}, _) as element =>
        let instance =
          createInstance(
            ~component=defaultHostComponent,
            ~element,
            ~instanceSubTree=IFlat([]),
            ~subElements=Flat([])
          );
        let instanceSubTree = renderReactElement(children);
        Instance({...instance, instanceSubTree, subElements: children})
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
      (~updateOpaqueInstanceState=?, ~updateLog, opaqueInstance, nextElement)
      : opaqueInstance => {
    let updatedOpaqueInstance =
      switch updateOpaqueInstanceState {
      | Some(f) => f(opaqueInstance)
      | None => opaqueInstance
      };
    let stateNotUpdated = opaqueInstance === updatedOpaqueInstance;
    let bailOut = {
      let Instance({element, component}) = opaqueInstance;
      let log = () => {
        logString(
          "Bailing Out Early Due To Memoization on " ++ component.debugName
        );
        true
      };
      stateNotUpdated && element === nextElement && log()
    };
    let logUpdate =
        (~componentChanged, ~stateChanged, ~subTreeChanged, newOpaqueInstance) => {
      let Instance({id}) = opaqueInstance;
      let Instance({id: newId}) = newOpaqueInstance;
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
      let Instance(
            {component, instanceSubTree, subElements} as updatedInstance
          ) = updatedOpaqueInstance;
      switch nextElement {
      | Element(nextComponent) =>
        component.handedOffInstance := Some(updatedInstance);
        switch nextComponent.handedOffInstance^ {
        /*
         * Case A: The next element *is* of the same component class.
         */
        | Some(updatedInstance) =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          component.handedOffInstance := None;
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
            let nextInstanceSubTree =
              updateRenderedElement(
                ~updateOpaqueInstanceState?,
                ~updateLog,
                (instanceSubTree, subElements, nextSubElements)
              );
            let newNewInstance = {
              ...newInstance,
              instanceSubTree: nextInstanceSubTree,
              subElements: nextSubElements
            };
            let newOpaqueInstance = Instance(newNewInstance);
            if (nextComponent.didUpdate !== defaultDidUpdate) {
              let newNewSelf = createSelf(~instance=newNewInstance);
              nextComponent.didUpdate({oldSelf, newSelf: newNewSelf})
            };
            let subTreeChanged = {
              let Instance({instanceSubTree}) = opaqueInstance;
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
            let newOpaqueInstance = Instance(newInstance);
            logUpdate(
              ~componentChanged=false,
              ~stateChanged,
              ~subTreeChanged=false,
              newOpaqueInstance
            );
            newOpaqueInstance
          }
        /*
         * Case B: The next element is *not* of the same component class. We know
         * because otherwise we would have observed the mutation on
         * `nextComponentClass`.
         */
        | None =>
          /* DO NOT FORGET TO RESET HANDEDOFFINSTANCE */
          component.handedOffInstance := None;
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
              component.debugName,
              nextComponent.debugName
            )
          );
          /* TODO: Invoke destruction lifecycle on previous component. */
          /* TODO: Invoke creation lifecycle on next component. */
          let nextSubtree = renderReactElement(nextSubElements);
          let newOpaqueInstance =
            Instance({
              ...nextInstance,
              instanceSubTree: nextSubtree,
              subElements: nextSubElements
            });
          logUpdate(
            ~componentChanged=true,
            ~stateChanged=true,
            ~subTreeChanged=true,
            newOpaqueInstance
          );
          newOpaqueInstance
        }
      | HostElement(_) => opaqueInstance
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
        (~keyTable, ~posTable, position, element)
        : opaqueInstance =>
      switch element {
      | Element(component) when component.key !== Key.none =>
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
      | Element(_) =>
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
      | HostElement(_) => renderElement(element)
      };
    switch (oldRenderedElement, oldReactElement, nextReactElement) {
    | (
        INested(iName, instanceSubTrees),
        Nested(_, oldReactElements),
        Nested(_, [nextReactElement, ...nextReactElements])
      )
        when nextReactElements === oldReactElements && GlobalState.useTailHack^ =>
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
        | None => OpaqueInstanceHash.createKeyTable(IFlat(oldOpaqueInstances))
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
    | _ =>
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
    let Instance(instance) = opaqueInstance;
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
    let pendingUpdates = List.rev(instance.pendingStateUpdates^);
    instance.pendingStateUpdates := [];
    let nextState = executeUpdates(~state=instance.iState, pendingUpdates);
    instance.iState === nextState ?
      opaqueInstance : Instance({...instance, iState: nextState})
  };

  /***
   * Flush the pending updates in an instance tree.
   * TODO: invoke lifecycles
   */
  let flushPendingUpdates = (~updateLog, opaqueInstance) => {
    let Instance({element}) = opaqueInstance;
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

let statelessComponent = (~useDynamicKey=?, debugName) => {
  ...basicComponent(~useDynamicKey?, debugName),
  initialState: () => ()
};

let statefulComponent = (~useDynamicKey=?, debugName) =>
  basicComponent(~useDynamicKey?, debugName);

let reducerComponent = (~useDynamicKey=?, debugName) =>
  basicComponent(~useDynamicKey?, debugName);

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
let nativeElement = (~key as argumentKey=Key.none, ~style=?, component) => {
  let key =
    argumentKey != Key.none ?
      argumentKey : component.nativeKey == Key.none ? Key.none : Key.create();
  let componentWithKey =
    key == Key.none ? component : {...component, nativeKey: key};
  Flat([
    HostElement(
      componentWithKey,
      switch style {
      | Some(style) => style
      | None => Layout.LayoutSupport.defaultStyle
      }
    )
  ])
};

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
    | Concrete(NativeView.t, Layout.LayoutSupport.LayoutTypes.cssStyle, node)
  and node = {
    mutable sub: list(tree),
    mutable id: int,
    mutable nearestParentView: NativeView.t
  };
  type viewHierarchy = {
    view: NativeView.t,
    children: list(viewHierarchy)
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
            print_endline("Unmounting a concrete view");
            ignore(NativeView.removeChild(node.nearestParentView, view))
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
          | Container(node) =>
            print_endline("Mounting a node lol");
            mountForest(node.sub)
          | Concrete(view, _, node) =>
            print_endline("Mounting a concrete view");
            mountForest(node.sub);
            ignore(NativeView.addChild(node.nearestParentView, view))
          },
        l
      )
    };
  type t = forest;
  let rec fromOpaqueInstance =
          (
            nearestParentView,
            Instance({component, element, iState, instanceSubTree, id})
          )
          : tree => {
    let subTreeInstances = Render.flattenRenderedElement(instanceSubTree);
    switch element {
    | HostElement(nativeComponent, style) =>
      let view =
        switch (NativeView.getInstance(id)) {
        | Some(x) => x
        | None => nativeComponent.make(id)
        };
      let sub = ListTR.map(fromOpaqueInstance(view), subTreeInstances);
      let node = {id, sub, nearestParentView};
      Concrete(view, style, node)
    | Element(_) =>
      let sub =
        ListTR.map(fromOpaqueInstance(nearestParentView), subTreeInstances);
      let node = {id, sub, nearestParentView};
      Container(node)
    }
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
            let Instance({component, iState, instanceSubTree}) = newOpaqueInstance;
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
              let Instance({instanceSubTree: oldInstanceSubTree}) = oldOpaqueInstance;
              let sub =
                switch (instanceSubTree, oldInstanceSubTree) {
                | (
                    INested(_, [nextRenderedElement, ...nextRenderedElements]),
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
    | Concrete(view, style, node) =>
      LayoutSupport.createNode(
        ~withChildren=Array.of_list(List.map(make, node.sub)),
        ~andStyle=style,
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
        ignore(
          NativeView.setFrame(
            node.layout.left,
            node.layout.top,
            node.layout.width,
            node.layout.height,
            view
          )
        )
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

module View = {
  type t = NativeView.t;
  let make = (children) => {
    name: "View",
    make: (id) => NativeView.makeInstance(id),
    setProps: (_) => (),
    children,
    nativeKey: Key.none
  };
};

module Button = {
  type t = NativeView.t;
  external makeInstance : int => NativeView.t = "Button_makeInstance";
  external setText : (string, t) => t = "Button_setText";
  [@noalloc] external setCallback : (unit => unit, t) => t =
    "Button_setCallback";
  let make = (~text, ~callback=?, children) => {
    name: "View",
    make: (id) => {
      let instance = makeInstance(id) |> setText(text);
      switch callback {
      | Some(callback) => setCallback(callback, instance)
      | None => instance
      }
    },
    setProps: (_) => (),
    children,
    nativeKey: Key.none
  };
};
