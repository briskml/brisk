open Brisk_macos;
open Components_macos;

let () = {
  open Cocoa;

  let appName = "BriskMac";

  Application.init();

  Application.willFinishLaunching(() => {
    let menu = MainMenu.make(appName);
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
      Main.(<app />),
    );

    RunLoop.spawn();
  });

  Application.run();
};
