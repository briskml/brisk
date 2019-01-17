type hook('a) = ..;

module Slots =
  Slots.Make({
    type t('a) = hook('a);
  });

type t('a, 'b) = {
  slots: Slots.t('a, 'b),
  onSlotsDidChange: unit => unit,
};

type empty = t(unit, unit);

type hooks('slots, 'nextSlots) = t('slots, 'nextSlots);

let create = (~onSlotsDidChange) => {
  slots: Slots.create(),
  onSlotsDidChange,
};

module State = {
  type t('a) = {
    mutable currentValue: 'a,
    mutable nextValue: 'a,
  };

  type hook('a) +=
    | State(t('a)): hook(t('a));

  let make: 'a => t('a) =
    initialValue =>
      {currentValue: initialValue, nextValue: initialValue};

  let flush: t('a) => bool =
    stateContainer => {
      let {currentValue, nextValue} = stateContainer;
      if (currentValue !== nextValue) {
        stateContainer.currentValue = nextValue;
        true;
      } else {
        false;
      };
    };

  let wrapAsHook = s => State(s);

  let setState = (nextValue, stateContainer) => {
    stateContainer.nextValue = nextValue;
  }

  let hook = (initialState, hooks) => {
    let (stateContainer, nextSlots) =
      Slots.use(
        ~default=() => make(initialState),
        ~toWitness=wrapAsHook,
        hooks.slots,
      );

    let setter = nextState => {
      setState(nextState, stateContainer);
      hooks.onSlotsDidChange();
    };

    (stateContainer.currentValue, setter, {...hooks, slots: nextSlots});
  };
};

module Reducer = {
  type t('a) = {
    mutable currentValue: 'a,
    mutable updates: list('a => 'a),
  };

  type hook('a) +=
    | Reducer(t('a)): hook(t('a));

  let make: 'a => t('a) =
    initialValue => {currentValue: initialValue, updates: []};

  let flush: t('a) => bool =
    reducerState => {
      let {currentValue, updates} = reducerState;
      let nextValue =
        List.fold_right(
          (update, latestValue) => update(latestValue),
          updates,
          currentValue,
        );

      reducerState.currentValue = nextValue;
      reducerState.updates = [];
      currentValue !== nextValue;
    };

  let wrapAsHook = s => Reducer(s);

  let enqueueUpdate = (nextUpdate, {updates} as stateContainer) => {
    stateContainer.updates = [nextUpdate, ...updates];
  };

  let hook = (~initialState, reducer, hooks: hooks(_)) => {
    let (stateContainer, nextSlots) =
      Slots.use(
        ~default=() => make(initialState),
        ~toWitness=wrapAsHook,
        hooks.slots,
      );

    let dispatch = action => {
      enqueueUpdate(prevValue => reducer(action, prevValue), stateContainer);
      hooks.onSlotsDidChange();
    };

    (stateContainer.currentValue, dispatch, {...hooks, slots: nextSlots});
  };
};

module Ref = {
  type t('a) = ref('a);
  type hook('a) +=
    | Ref(t('a)): hook(t('a));
  let wrapAsHook = s => Ref(s);

  let hook = (initialState, hooks) => {
    let (internalRef, nextSlots) =
      Slots.use(
        ~default=() => ref(initialState),
        ~toWitness=wrapAsHook,
        hooks.slots,
      );

    let setter = nextValue => internalRef := nextValue;

    (internalRef^, setter, {...hooks, slots: nextSlots});
  };
};

module Effect = {
  type lifecycle =
    | Mount
    | Unmount
    | Update;
  type always;
  type onMount;
  type condition('a) =
    | Always: condition(always)
    | OnMount: condition(onMount)
    | If(('a, 'a) => bool, 'a): condition('a);
  type handler = unit => option(unit => unit);
  type t('a) = {
    mutable condition: condition('a),
    mutable handler: unit => option(unit => unit),
    mutable cleanupHandler: option(unit => unit),
    mutable previousCondition: condition('a),
  };
  type hook('a) +=
    | Effect(t('a)): hook(t('a));

  let wrapAsHook = s => Effect(s);

  let executeOptionalHandler =
    fun
    | Some(f) => {
        f();
        true;
      }
    | None => false;

  let executeIfNeeded:
    type conditionValue. (~lifecycle: lifecycle, t(conditionValue)) => bool =
    (~lifecycle, state) => {
      let {condition, previousCondition, handler, cleanupHandler} = state;
      switch (previousCondition) {
      | Always =>
        ignore(executeOptionalHandler(cleanupHandler));
        state.cleanupHandler = handler();
        true;
      | If(comparator, previousConditionValue) =>
        switch (lifecycle) {
        | Mount
        | Update =>
          let currentConditionValue =
            switch (condition) {
            | If(_, currentConditionValue) => currentConditionValue
            /* The following cases are unreachable because it's
             * Impossible to create a value of type condition(always)
             * Or condition(onMount) using the If constructor
             */
            | Always => previousConditionValue
            | OnMount => previousConditionValue
            };
          if (comparator(previousConditionValue, currentConditionValue)) {
            ignore(executeOptionalHandler(cleanupHandler));
            state.previousCondition = condition;
            state.cleanupHandler = handler();
            true;
          } else {
            state.previousCondition = condition;
            false;
          };
        | Unmount => executeOptionalHandler(cleanupHandler)
        }
      | OnMount =>
        switch (lifecycle) {
        | Mount =>
          state.cleanupHandler = handler();
          true;
        | Unmount => executeOptionalHandler(cleanupHandler)
        | _ => false
        }
      };
    };

  let hook = (condition, handler, hooks) => {
    let (state, nextSlots) =
      Slots.use(
        ~default=
          () => {
            condition,
            handler,
            cleanupHandler: None,
            previousCondition: condition,
          },
        ~toWitness=wrapAsHook,
        hooks.slots,
      );

    state.condition = condition;
    state.handler = handler;
    {...hooks, slots: nextSlots};
  };
};

let state = State.hook;
let reducer = Reducer.hook;
let ref = Ref.hook;
let effect = Effect.hook;

let executeEffects = (~lifecycle, {slots}) =>
  Slots.fold(
    (Any(hook), shouldUpdate) =>
      switch (hook) {
      | Effect.Effect(state) =>
        Effect.executeIfNeeded(~lifecycle, state) || shouldUpdate
      | _ => shouldUpdate
      },
    false,
    slots,
  );

let flushPendingStateUpdates = ({slots}) =>
  Slots.fold(
    (Any(hook), shouldUpdate) =>
      switch (hook) {
      | State.State(state) => State.flush(state) || shouldUpdate
      | Reducer.Reducer(state) => Reducer.flush(state) || shouldUpdate
      | _ => shouldUpdate
      },
    false,
    slots,
  );
