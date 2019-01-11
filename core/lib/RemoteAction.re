type t('action) = {mutable send: 'action => unit};
let sendDefault = _action => ();
let create = () => {send: sendDefault};
let subscribe = (~send, x) =>
  if (x.send === sendDefault) {
    x.send = send;
  };
let send = (x, ~action) => x.send(action);
