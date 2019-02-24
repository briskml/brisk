open Brisk_macos;

module Tab = {
  type t =
    | Top
    | Show
    | Newest
    | Jobs;

  let title =
    fun
    | Top => "Top"
    | Show => "Show"
    | Newest => "Newest"
    | Jobs => "Jobs";
};

type tabItem = {
  kind: Tab.t,
  label: string,
};

let tabs =
  Tab.[Top, Show, Newest, Jobs]
  |> List.map(kind => {kind, label: Tab.title(kind)});

type state = {currentTab: Tab.t};

let examples = {
  let id = Brisk.component("examples");

  (~children as _: list(unit), ()) =>
    id(hooks => {
      open Brisk.Layout;
      let (state, setState, hooks) =
        Brisk.Hooks.state({currentTab: Top}, hooks);

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
                              tab.kind === currentTab
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
