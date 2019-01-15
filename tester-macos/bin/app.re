open Brisk_renderer_macos;
open Cocoa;
open Layout;
open Lwt.Infix;

module Component = {
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
            position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
            width(600.),
            height(400.),
            background(Color.hex("#f7f8f9")),
          ]>
          <Text
            style=[
              font(~size=24., ~weight=`Medium, ()),
              kern(0.5),
              align(`Center),
              color(Color.hex("#ffffff")),
              background(Color.hex("#263ac5")),
              padding(10.),
            ]
            value="Welcome to Brisk"
          />
          <View
            style=[
              justifyContent(`Center),
              alignContent(`Center),
              background(Color.hex("#eeeeee")),
            ]>
            <Image
              style=[margin4(~top=10., ()), alignSelf(`Center)]
              source={`Bundle("reason")}
            />
            <Text
              style=[
                font(~size=18., ()),
                align(`Center),
                alignSelf(`Center),
                width(200.),
                cornerRadius(10.),
                color(Color.hex("#ffffff")),
                background(Color.hexa("#263ac5", 0.9)),
                margin(20.),
                padding2(~h=10., ~v=10., ()),
              ]
              value="Text bubble"
            />
          </View>
          <Button
            style=[width(400.), height(60.)]
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
      let layoutNode =
        makeLayoutNode(
          ~style=[width(window#contentWidth), height(window#contentHeight)],
          view,
        );
      {React.NativeCocoa.view, layoutNode};
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
  });

  NSApplication.main();
};
