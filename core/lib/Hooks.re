let useReducer:
  type state action.
    (
      ~initialState: state,
      (action, state) => state,
      Slots.t(state, Slots.t('slot, 'nextSlots))
    ) =>
    (state, action => unit, Slots.t('slot, 'nextSlots)) =
  (~initialState, reducer, slots) => {
    let ((state, setState), nextSlots) =
      Slots.use(~default=initialState, slots);

    let dispatch = (action: action) =>
      setState(prevValue => reducer(action, prevValue));

    (state, dispatch, nextSlots);
  };

let useState:
  type state.
    (state, Slots.t(state, Slots.t('slot, 'nextSlots))) =>
    (state, state => unit, Slots.t('slot, 'nextSlots)) =
  (initialState, slots) => {
    let ((state, setState), nextSlots) =
      Slots.use(~default=initialState, slots);

    let setter = (nextState: state) => setState(_ => nextState);

    (state, setter, nextSlots);
  };

let useRef:
  type state.
    (state, Slots.t(ref(state), Slots.t('slot, 'nextSlots))) =>
    (state, state => unit, Slots.t('slot, 'nextSlots)) =
  (initialState, slots) => {
    let ((state, _), nextSlots) =
      Slots.use(~default=ref(initialState), slots);

    let setter = nextValue =>
      /* do this after all updates are commited to the OutputTree */
      state := nextValue;

    (state^, setter, nextSlots);
  };
