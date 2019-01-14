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
    (
      (opaqueElement, 'acc, ~flush: unit => bool) => 'acc,
      'acc,
      t('slots, 'nextSlots)
    ) =>
    'acc;
};

module Make: (Elem: Elem) => S with type elem('a) = Elem.t('a);

include S with type elem('a) = 'a;
