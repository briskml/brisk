open Brisk_macos;

module Tab = {
  type t =
    | Top
    | Show
    | Newest
    | Jobs;

  let title =
    fun
    | Top => "op"
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
  let id = Brisk.component("app");

  (~children as _: list(unit), ()) =>
    id(hooks =>
      Brisk.Layout.(
        hooks,
        <view
          style=[
            position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
            flexDirection(`Row),
          ]>
          <effectView
            style=EffectView.[blendingMode(`BehindWindow), width(74.)]
          />
          <scrollView style=[flex(1.), padding4(~top=20., ~left=13., ())]>
            TopStories.component
          </scrollView>
        </view>,
      )
    );
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
