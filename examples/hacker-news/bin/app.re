open Brisk_macos;

type tab =
  | Welcome
  | Views
  | Buttons
  | Text
  | Image
  | System;

type tabItem = {
  kind: tab,
  label: string,
};

let tabs = [
  {kind: Welcome, label: "Welcome"},
  {kind: Views, label: "Views"},
  {kind: Buttons, label: "Buttons"},
  {kind: Text, label: "Text"},
  {kind: Image, label: "Image"},
];

type state = {currentTab: tab};

let examples = {
  let id = Brisk.component("examples");

  (~children as _, ()) =>
    id(hooks => {
      open Brisk.Layout;
      let (state, setState, hooks) =
        Brisk.Hooks.state({currentTab: Welcome}, hooks);

      let {currentTab} = state;

      (
        hooks,
        <view
          style=[
            position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
            flexDirection(`Row),
          ]>
          <effectView
            style=EffectView.[
              blendingMode(`BehindWindow),
              position(~top=0., ~left=0., ~bottom=0., `Absolute),
              width(185.),
            ]>
            <view style=[margin2(~v=37., ())]>
              ...{
                   tabs
                   |> List.map(tab =>
                        <text
                          style=[
                            font(~size=15., ()),
                            kern(0.09),
                            align(`Left),
                            color(Color.hex("#282522")),
                            background(
                              tab.kind == currentTab
                                ? Color.hexa("#000000", 0.4)
                                : Color.transparent,
                            ),
                            padding2(~h=25., ~v=4., ()),
                          ]
                          value={tab.label}
                        />
                      )
                 }
            </view>
          </effectView>
          <scrollView
            style=[
              position(~top=0., ~left=185., ~right=0., ~bottom=0., `Absolute),
              padding4(~top=20., ~left=13., ()),
            ]>
            <text
              style=[
                font(~size=24., ~weight=`Semibold, ()),
                kern(0.58),
                align(`Left),
                color(Color.hex("#000000")),
                background(Color.transparent),
              ]
              value="Welcome to Brisk"
            />
            <view style=[justifyContent(`Center), alignContent(`Center)]>
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
                    >>= (_ => Lwt.return(setState(state))),
                  )
                )
              }
            />
            <view style=[alignContent(`Center), height(600.)]>
              <text
                style=[
                  font(~size=18., ()),
                  align(`Center),
                  alignSelf(`Center),
                  width(200.),
                  height(300.),
                  border(~radius=10., ()),
                  color(Color.hex("#011021")),
                  margin(20.),
                  padding2(~h=10., ~v=10., ()),
                ]
                value="Very large height for scrolling"
              />
            </view>
          </scrollView>
        </view>,
      );
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
    open Brisk;

    let view = BriskView.make();
    let window =
      Window.make(
        ~width=680.,
        ~height=468.,
        ~title=appName,
        ~contentView=view,
        ~contentIsFullSize=true,
        ~onResize=win => UI.setWindowHeight(Window.contentHeight(win)),
        (),
      );

    /* Make the window 'frameless' */
    Window.setTitleIsHidden(window, true);
    Window.setTitlebarIsTransparent(window, true);

    let root = {
      open Brisk.Layout;

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

    UI.renderAndMount(
      ~height=Window.contentHeight(window),
      root,
      <examples />,
    );

    RunLoop.spawn();
  });

  Application.run();
};
