let make = (callback, ()) => {
  callback();
  /* This is a workaround for non-deterministic crashes 
   * when OCaml wants to allocate huge amounts of memory.
   * This crash still happens but is a lot less frequent.
   * A very negative side effect is terrible performance.
   */
  Gc.minor();
  Brisk.UI.flushPendingUpdates();
  Brisk.UI.executeHostViewUpdatesAndLayout();
  LwtFakeIOEvent.send();
};
