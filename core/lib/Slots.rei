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

module Make: (Witness: Witness) => S with type witness('a) = Witness.t('a);

include S with type witness('a) = 'a;
