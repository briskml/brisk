open Brisk_macos;

let component = Brisk.component("Other");
let component = (~children as _, ()) =>
  component(hooks => {
    open Brisk.Layout;
    let (state, setState, hooks) = Brisk.Hooks.state(None, hooks);
    (
      hooks,
      switch (state) {
      | Some(code) =>
        <effectView
          style=EffectView.[
            position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
            blendingMode(`BehindWindow),
          ]>
          <button
            style=[width(100.), height(100.), align(`Center)]
            title={string_of_int(code)}
            callback={() => setState(None)}
          />
          <button
            style=[width(100.), height(100.), align(`Center)]
            title="Cell two"
            callback={() => setState(None)}
          />
        </effectView>
      | None =>
        <scrollView
          style=[
            position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
            background(Color.hex("#f7f8f9")),
          ]>
          <text
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
          <view
            style=[
              justifyContent(`Center),
              alignContent(`Center),
              background(Color.hex("#eeeeee")),
            ]>
            <image
              style=[margin4(~top=10., ()), alignSelf(`Center)]
              source={`Bundle("reason")}
            />
            <text
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
          </view>
          <button
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
          <view style=[alignContent(`Center), height(900.)]>
            <text
              style=[
                font(~size=18., ()),
                align(`Center),
                alignSelf(`Center),
                width(200.),
                height(600.),
                border(~radius=10., ()),
                color(Color.hex("#011021")),
                background(Color.hex("#f0f0f0")),
                margin(20.),
                padding2(~h=10., ~v=10., ()),
              ]
              value="Very large height for scrolling"
            />
          </view>
        </scrollView>
      },
    );
  });

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
      open Brisk.Layout;
      let view = BriskView.make();

      let layoutNode =
        Node.make(
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
      <component />,
    );

    Brisk.RunLoop.spawn();
  });

  Application.run();
};
