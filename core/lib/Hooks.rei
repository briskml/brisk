type hook('a) = ..;

module Slots: Slots.S with type witness('a) = hook('a);

type t('a, 'b) = {
  slots: Slots.t('a, 'b),
  onSlotsDidChange: unit => unit,
};
type empty = t(unit, unit);

let create: (~onSlotsDidChange: unit => unit) => t('a, 'b);

module State: {
  type t('a);
  type hook('a) +=
    pri
    | State(t('a)): hook(t('a));
};

module Reducer: {
  type t('a);
  type hook('a) +=
    pri
    | Reducer(t('a)): hook(t('a));
};

module Ref: {
  type t('a);
  type hook('a) +=
    pri
    | Ref(t('a)): hook(t('a));
};

module Effect: {
  type t('a);
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
  type hook('a) +=
    pri
    | Effect(t('a)): hook(t('a));
};

let state:
  ('state, t(State.t('state), Slots.t('slots, 'nextSlots))) =>
  ('state, 'state => unit, t('slots, 'nextSlots));

let reducer:
  (
    ~initialState: 'state,
    ('action, 'state) => 'state,
    t(Reducer.t('state), Slots.t('slots, 'nextSlots))
  ) =>
  ('state, 'action => unit, t('slots, 'nextSlots));

let ref:
  ('state, t(Ref.t('state), Slots.t('slots, 'nextSlots))) =>
  ('state, 'state => unit, t('slots, 'nextSlots));

let effect:
  (
    Effect.condition('condition),
    Effect.handler,
    t(Effect.t('condition), Slots.t('slots, 'nextSlots))
  ) =>
  t('slots, 'nextSlots);

let executeEffects: (~lifecycle: Effect.lifecycle, t('a, 'b)) => bool;
let flushPendingStateUpdates: t('a, 'b) => bool;
