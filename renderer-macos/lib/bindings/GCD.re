external dispatchAsyncBackground: (unit => unit) => unit =
  "ml_dispatchAsyncBackground";
external dispatchSyncMain: (unit => unit) => unit = "ml_dispatchSyncMain";
[@noalloc]
external isIsMainThread: unit => [@untagged] int =
  "ml_isMainThread" "ml_isMainThread";
