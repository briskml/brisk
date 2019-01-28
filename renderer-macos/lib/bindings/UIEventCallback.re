let make = (callback, ()) => {
  callback();
  Brisk.UI.flushPendingUpdates();
  Brisk.UI.executeHostViewUpdatesAndLayout();
  LwtFakeIOEvent.send();
};
