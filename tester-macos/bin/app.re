open Brisk_renderer_macos;
open Cocoa;
open Lwt.Infix;

module Component = {
  open Layout;
  [@noalloc] external lwt_start: unit => unit = "ml_lwt_iter";

  let otherComponent = React.reducerComponent("Other");
  let createElement = (~children as _, ()) => {
    ...otherComponent,
    initialState: _ => None,
    reducer: (x, _) => React.Update(x),
    render: ({state, reduce}) =>
      switch (state) {
      | Some(code) =>
        <View
          style=[
            width(100.),
            height(100.),
            background(Color.rgb(0, 255, 0)),
            border(~width=1., ~color=Color.rgb(0, 0, 255), ()),
          ]>
          <Button
            style=[width(100.), height(100.)]
            title={string_of_int(code)}
            callback={reduce(() => None)}
          />
          <Button
            style=[width(100.), height(100.)]
            title="Cell two"
            callback={reduce(() => None)}
          />
        </View>
      | None =>
        <View
          style=[
            height(400.),
            background(Color.rgb(255, 0, 0)),
            border(~width=1., ~color=Color.rgb(0, 255, 0), ()),
          ]>
          <Button
            style=[width(600.), height(40.)]
            title="Youre gonna have to wait a bit"
            callback={
                       let callback = reduce(code => code);
                       (
                         () => {
                           lwt_start();
                           ignore(
                             Lwt_unix.sleep(1.)
                             >>= (_ => Lwt.return(callback(Some(100)))),
                           );
                         }
                       );
                     }
          />
        </View>
      },
  };
};

let lwt_iter = () => {
  Lwt.wakeup_paused();
  Lwt_engine.iter(false);
  Lwt.wakeup_paused();
};

let () = {
  let appName = "BriskMac";

  Callback.register("Brisk_flush_layout", React.RunLoop.flushAndLayout);
  Callback.register("Brisk_lwt_iter", lwt_iter);

  NSApplication.init();

  NSApplication.willFinishLaunching(() => {
    let menu = Menu.makeMainMenu(appName);
    CocoaMenu.NSMenu.add(~kind=Main, menu);
  });

  NSApplication.didFinishLaunching(() => {
    let window = NSWindow.makeWithContentRect(0., 0., 680., 468.);

    let root = {
      let view = NSView.make();
      {
        React.NativeCocoa.view,
        layoutNode:
          Layout.LayoutSupport.createNode(
            ~withChildren=[||],
            ~andStyle={
              ...Layout.LayoutSupport.defaultStyle,
              width: 400,
              height: 460,
            },
            view,
          ),
      };
    };

    window#center;
    window#makeKeyAndOrderFront;
    window#setTitle(appName);
    window#setContentView(root.view);

    window#windowDidResize(_ =>
      React.RunLoop.setWindowHeight(window#contentHeight)
    );

    React.RunLoop.renderAndMount(
      ~height=window#contentHeight,
      root,
      React.element(<Component />),
    );

    open Cf.RunLoop;
    /* MAIN THREAD */

    let observer =
      Observer.(create(Activity.All, _ => React.RunLoop.flushAndLayout()));
    let runloop = get_current();
    add_observer(runloop, observer, Mode.Default);
    /* START APP LOGIC ON ANOTHER THREAD AND SETUP LWT THERE */
    /* Lwt_main.run(
      Cf_lwt.RunLoop.run_thread(_ =>
        Lwt_preemptive.run_in_main(_ => {
          lwt_iter();
          Lwt.return_unit;
        })
      )
      >>= (_ => Lwt.return_unit),
    ); */
  });

  NSApplication.main();
};
