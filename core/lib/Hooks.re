let useReducer = (~initialState, reducer, slots) => {
  let ((state, setState), nextSlots) =
    Slots.use(~default=initialState, slots);

  let dispatch = action => setState(prevValue => reducer(action, prevValue));

  (state, dispatch, nextSlots);
};

let useState = (initialState, slots) => {
  let ((state, setState), nextSlots) =
    Slots.use(~default=initialState, slots);

  let setter = nextState => setState(_ => nextState);

  (state, setter, nextSlots);
};

let useRef = (initialState, slots) => {
  let ((state, _), nextSlots) =
    Slots.use(~default=ref(initialState), slots);

  let setter = nextValue =>
    /* do this after all updates are commited to the OutputTree */
    state := nextValue;

  (state^, setter, nextSlots);
};
