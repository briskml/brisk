type slotInternal('slot, 'nextSlots) = {
  flush: unit => ('slot, bool),
  this: 'slot,
  next: 'nextSlots,
};

type t('slot, 'nextSlots) = ref(option(slotInternal('slot, 'nextSlots)));

type empty = t(unit, unit);

let create = () => ref(None);

let flushPendingUpdatesInternal = (default, slots) =>
  switch (slots) {
  | Some({flush}) => flush()
  | None => (default, false)
  };

let flushPendingUpdates = slots =>
  switch (slots^) {
  | Some({flush}) => snd(flush())
  | None => false
  };

let shouldUpdate = (slots, newValue) =>
  switch (slots) {
  | Some({this}) => this !== newValue
  | None => true
  };

let stabilized = (this, nextSlots) => {
  this,
  next: nextSlots,
  flush: () => (this, false),
};

let makeSetter =
    (~flushed, ~default, ~latestSlotsRef, ~nextSlots, makeNewValue) => {
  let prevSlots = latestSlotsRef^;
  latestSlotsRef :=
    Some({
      ...flushed,
      flush: () => {
        let prevValue = fst(flushPendingUpdatesInternal(default, prevSlots));
        let newValue = makeNewValue(prevValue);
        let shouldUpdate = shouldUpdate(prevSlots, newValue);
        latestSlotsRef := Some(stabilized(newValue, flushed.next));
        (newValue, flushPendingUpdates(nextSlots) || shouldUpdate);
      },
    });
};

let use = (~default, slots: t(_)) =>
  switch (slots^) {
  | None =>
    let nextSlots = create();
    let slot = stabilized(default, nextSlots);
    slots := Some(slot);
    (
      (
        slot.this,
        makeSetter(
          ~flushed=slot,
          ~default,
          ~latestSlotsRef=slots,
          ~nextSlots,
        ),
      ),
      nextSlots,
    );
  | Some(slot) => (
      (
        slot.this,
        makeSetter(
          ~flushed=slot,
          ~default,
          ~latestSlotsRef=slots,
          ~nextSlots=slot.next,
        ),
      ),
      slot.next,
    )
  };
