type t('slot, 'nextSlots);
let create: unit => t('slot, 'nextSlots);
let use:
  (~default: 'slot, t('slot, t('slot2, 'nextSlots))) =>
  (('slot, ('slot => 'slot) => unit), t('slot2, 'nextSlots));
let flushPendingUpdates: t('slot, 'nextSlots) => bool;

type empty = t(unit, unit);
