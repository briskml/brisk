open Brisk_macos;
open Layout;

module Component = {
  let component = Brisk.component("Other");
  let createElement = (~children as _, ()) =>
    component(slots => {
      let (state, setState, _slots: Brisk.Hooks.empty) =
        Brisk.Hooks.state(None, slots);

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
            style=[width(100.), height(100.), align(`Center)]
            title={string_of_int(code)}
            callback={() => setState(None)}
          />
          <Button
            style=[width(100.), height(100.), align(`Center)]
            title="Cell two"
            callback={() => setState(None)}
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
                border(~radius=10., ()),
                color(Color.hex("#ffffff")),
                background(Color.hexa("#263ac5", 0.9)),
                margin(20.),
                padding2(~h=10., ~v=10., ()),
              ]
              value="Text bubble"
            />
          </View>
          <Button
            style=[
              width(400.),
              height(60.),
              margin4(~top=20., ()),
              alignSelf(`Center),
              font(~size=16., ()),
              color(Color.hex("#ffffff")),
              background(Color.hex("#263ac5")),
              align(`Center),
            ]
            title="You're gonna have to wait 1 second"
            callback={() =>
              Lwt.Infix.(
                ignore(
                  Lwt_unix.sleep(1.)
                  >>= (_ => Lwt.return(setState(Some(100)))),
                )
              )
            }
          />
        </View>
      };
    });
};

let () = {
  open Cocoa;

  let appName = "BriskMac";

  Application.init();

  Application.willFinishLaunching(() => {
    let menu = UiMenu.makeMainMenu(appName);
    Menu.add(~kind=Main, menu);
  });

  Application.didFinishLaunching(() => {
    let window = Window.makeWithContentRect(0., 0., 680., 468.);

    let root = {
      let view = BriskView.make();
      let layoutNode =
        makeLayoutNode(
          ~style=[
            width(Window.contentWidth(window)),
            height(Window.contentHeight(window)),
          ],
          view,
        );
      {Brisk.OutputTree.view, layoutNode};
    };

    Window.center(window);
    Window.makeKeyAndOrderFront(window);
    Window.setTitle(window, appName);
    Window.setContentView(window, root.view);

    Window.windowDidResize(window, _ =>
      Brisk.UI.setWindowHeight(Window.contentHeight(window))
    );

    Brisk.UI.renderAndMount(
      ~height=Window.contentHeight(window),
      root,
      Brisk.element(<Component />),
    );

    Brisk.RunLoop.spawn();
  });

  Application.run();
};
