  external dispatchAsyncBackground: (unit => unit) => unit = "ml_dispatchAsyncBackground";
  external dispatchSyncMain: (unit => unit) => unit = "ml_dispatchSyncMain";
  external printIsMain: unit => unit = "ml_printIsMain";
