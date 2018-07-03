open Brisk_cocoa;

let render = () => {
  open Cocoa;

  let app = Lazy.force(NSApplication.app);

  app#applicationWillFinishLaunching(_ => {
    let rect = (0., 0., 400., 400.);
    let w = NSWindow.windowWithContentRect(rect);
    let i = ref(0);
    w#windowDidResize(_ => {
      log("Window Did Resize");
      incr(i);
      w#setTitle("Resized " ++ string_of_int(i^) ++ " times");
    });
    w#center;
    w#makeKeyAndOrderFront;
  });
  app#run;
};

let () = React.run(render);
