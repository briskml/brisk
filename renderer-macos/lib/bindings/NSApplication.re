type t = CocoaClass.application;

external init: unit => unit = "ml_NSApplication_configure";

external main: unit => unit = "ml_NSApplication_main";

let isInitialized = ref(false);
let init = () =>
  if (isInitialized^ === false) {
    init();
    isInitialized := true;
  };

let identity_f = () => ();

exception NSAppNotInitialized;

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
