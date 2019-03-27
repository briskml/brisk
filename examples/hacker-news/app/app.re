open Brisk_macos;

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
        ~contentView=view,
        ~contentIsFullSize=false,
        ~onResize=win => UI.setWindowHeight(Window.contentHeight(win)),
        (),
      );

    Window.setTitleIsHidden(window, true);


    let root = {
      open Brisk.Layout;

      let layoutNode =
        Node.make(
          ~style=[
            width(Window.contentWidth(window)),
            height(Window.contentHeight(window)),
          ],
          {view, isYAxisFlipped: false},
        );
      {Brisk.OutputTree.view, layoutNode};
    };

    UI.renderAndMount(
      ~height=Window.contentHeight(window),
      root,
      Stories.component,
    );

    RunLoop.spawn();
  });

  Application.run();
};
