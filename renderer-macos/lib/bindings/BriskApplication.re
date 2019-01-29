type t = CocoaTypes.application;

exception NSAppNotInitialized;

external init: unit => unit = "ml_NSApplication_init";

external run: unit => unit = "ml_NSApplication_run";

let isInitialized = ref(false);

let willTerminate = func => {
  if (! isInitialized^) {
    raise(NSAppNotInitialized);
  };
  Callback.register("NSAppDelegate.applicationWillTerminate", func);
};

let willFinishLaunching = func => {
  if (! isInitialized^) {
    raise(NSAppNotInitialized);
  };
  Callback.register("NSAppDelegate.applicationWillFinishLaunching", func);
};

let didFinishLaunching = func => {
  if (! isInitialized^) {
    raise(NSAppNotInitialized);
  };
  Callback.register("NSAppDelegate.applicationDidFinishLaunching", func);
};

let init = () =>
  if (isInitialized^ === false) {
    init();
    isInitialized := true;
  };

let run = () => {
  if (! isInitialized^) {
    raise(NSAppNotInitialized);
  };
  run();
};
