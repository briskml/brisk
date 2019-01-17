module type Witness = {type t('a);};

module type S = {
  type witness('a);
  type opaqueValue =
    | Any(witness('a)): opaqueValue;
  type t('slot, 'nextSlots);
  type empty = t(unit, unit);

  let create: unit => t('slot, 'nextSlots);
  let use:
    (
      ~default: unit => 'slot,
      ~toWitness: 'slot => witness('slot),
      t('slot, t('slot2, 'nextSlots))
    ) =>
    ('slot, t('slot2, 'nextSlots));

  let fold:
    ((opaqueValue, 'acc) => 'acc, 'acc, t('slots, 'nextSlots)) => 'acc;
};

module Make = (Witness: Witness) => {
  type witness('a) = Witness.t('a);

  type opaqueValue =
    | Any(Witness.t('a)): opaqueValue;

  type slotInternal('slot, 'nextSlots) = {
    value: 'slot,
    fold: 'acc. ((opaqueValue, 'acc) => 'acc, 'acc) => 'acc,
    next: 'nextSlots,
  };

  type t('slot, 'nextSlots) = ref(option(slotInternal('slot, 'nextSlots)));

  type empty = t(unit, unit);

  let create = () => ref(None);

  let use:
    (
      ~default: unit => 'slot,
      ~toWitness: 'slot => witness('slot),
      t('slot, t('slot2, 'nextSlots))
    ) =>
    ('slot, t('slot2, 'nextSlots)) =
    (~default, ~toWitness, slots) =>
      switch (slots^) {
      | None =>
        let nextSlots = create();
        let value = default();
        slots :=
          Some({
            value,
            next: nextSlots,
            fold: (f, initialValue) => {
              let acc = f(Any(toWitness(value)), initialValue);
              switch (nextSlots^) {
              | Some({fold}) => fold(f, acc)
              | None => acc
              };
            },
          });
        (value, nextSlots);
      | Some({value, next}) => (value, next)
      };

  let fold = (f, initialValue, slots) =>
    switch (slots^) {
    | Some({fold}) => fold(f, initialValue)
    | None => initialValue
    };
};

include Make({
  type t('a) = 'a;
});
