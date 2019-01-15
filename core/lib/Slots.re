module type Elem = {type t('a);};

module type S = {
  type elem('a);
  type opaqueElement =
    | Any(elem('a)): opaqueElement;
  type t('slot, 'nextSlots);
  type empty = t(unit, unit);

  let create: unit => t('slot, 'nextSlots);
  let use:
    (
      ~default: unit => 'slot,
      ~toElem: 'slot => elem('slot),
      t('slot, t('slot2, 'nextSlots))
    ) =>
    ('slot, t('slot2, 'nextSlots));

  let fold:
    ((opaqueElement, 'acc) => 'acc, 'acc, t('slots, 'nextSlots)) => 'acc;
};

module Make = (Elem: Elem) => {
  type elem('a) = Elem.t('a);

  type opaqueElement =
    | Any(Elem.t('a)): opaqueElement;

  type slotInternal('slot, 'nextSlots) = {
    value: 'slot,
    fold: 'acc. ((opaqueElement, 'acc) => 'acc, 'acc) => 'acc,
    next: 'nextSlots,
  };

  type t('slot, 'nextSlots) = ref(option(slotInternal('slot, 'nextSlots)));

  type empty = t(unit, unit);

  let create = () => ref(None);

  let use:
    (
      ~default: unit => 'slot,
      ~toElem: 'slot => elem('slot),
      t('slot, t('slot2, 'nextSlots))
    ) =>
    ('slot, t('slot2, 'nextSlots)) =
    (~default, ~toElem, slots) =>
      switch (slots^) {
      | None =>
        let nextSlots = create();
        let value = default();
        slots :=
          Some({
            value,
            next: nextSlots,
            fold: (f, initialValue) => {
              let acc = f(Any(toElem(value)), initialValue);
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
