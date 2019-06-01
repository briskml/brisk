let () = {
  open Brisk_macos.Cocoa;

  Application.init();

  Application.willFinishLaunching(() => {
    open Brisk_macos.Menu;
    setServicesMenu(MenuBar.servicesMenu);
    setWindowsMenu(MenuBar.windowsMenu);
    setMainMenu(MenuBar.menu);
  });

  Application.didFinishLaunching(() => {
    let view = BriskView.make();
    let window =
      Window.make(
        ~width=680.,
        ~height=468.,
        ~contentView=view,
        ~contentIsFullSize=false,
        ~onResize=
          win =>
            Brisk_macos.UIRunner.setWindowHeight(Window.contentHeight(win)),
        (),
      );

    Window.setTitleIsHidden(window, true);

    RunLoop.renderAndMount(
      {window, contentView: view},
      Stories.storiesScreen,
    );

    RunLoop.spawn();
  })

  Application.run();
};
