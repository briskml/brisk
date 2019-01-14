module type Elem = {type t('a);};

module type S = {
  type elem('a);
  type opaqueElement =
    | Any(elem('a)): opaqueElement;
  type t('slot, 'nextSlots);
  type empty = t(unit, unit);

  let create: unit => t('slot, 'nextSlots);
  let use:
    (~default: elem('slot), t('slot, t('slot2, 'nextSlots))) =>
    (
      (elem('slot), (elem('slot) => elem('slot)) => unit),
      t('slot2, 'nextSlots),
    );

  let fold:
    ((opaqueElement, 'a, ~flush: unit => bool) => 'a, 'a, t('b, 'c)) => 'a;
};

module Make = (Elem: Elem) => {
  type elem('a) = Elem.t('a);

  type opaqueElement =
    | Any(Elem.t('a)): opaqueElement;

  type updates('a) = list('a => 'a);
  type value('a) = ref(('a, updates('a)));

  type slotInternal('slot, 'nextSlots) = {
    fold:
      'acc.
      ((opaqueElement, 'acc, ~flush: unit => bool) => 'acc, 'acc) => 'acc,

    currentValue: value(Elem.t('slot)),
    enqueueUpdate: (Elem.t('slot) => Elem.t('slot)) => unit,
    next: 'nextSlots,
  };

  type t('slot, 'nextSlots) = ref(option(slotInternal('slot, 'nextSlots)));

  type empty = t(unit, unit);

  let create = () => ref(None);

  let use = (~default, slots) =>
    switch (slots^) {
    | None =>
      let nextSlots = create();
      let currentValue = ref((default, []));
      let enqueueUpdate = f => {
        let (cv, t) = currentValue^;
        currentValue := (cv, [f, ...t]);
      };
      let flush = () => {
        let (prevValue, updates) = currentValue^;
        let nextValue =
          List.fold_right(
            (update, latestValue) => update(latestValue),
            updates,
            prevValue,
          );
        currentValue := (nextValue, []);
        nextValue === prevValue ? false : true;
      };
      slots :=
        Some({
          currentValue,
          next: nextSlots,
          enqueueUpdate,
          fold: (f, initialValue) => {
            let acc = f(Any(fst(currentValue^)), initialValue, ~flush);
            switch (nextSlots^) {
            | Some({fold}) => fold(f, acc)
            | None => acc
            };
          },
        });
      ((default, enqueueUpdate), nextSlots);
    | Some({currentValue, enqueueUpdate, next}) =>
      let (currentValue, _) = currentValue^;
      ((currentValue, enqueueUpdate), next);
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
