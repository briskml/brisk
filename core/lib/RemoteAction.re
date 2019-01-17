type t('action) = {mutable subscribers: list('action => unit)};

type unsubscribe = unit => unit;

let create = () => {subscribers: []};

let subscribe = (~handler: 'action => unit, {subscribers} as emitter: t('a)) => {
  if (!List.exists(f => f === handler, subscribers)) {
    emitter.subscribers = [handler, ...subscribers];
  }
  let unsubscribe = () => {
    emitter.subscribers = List.filter(f => f !== f, subscribers);
  };
  unsubscribe;
};

let send = (~action: 'a, emitter: t('a)) => {
  List.iter(c => c(action), emitter.subscribers);
}
