open Brisk_macos;

let () = {
  open Cocoa;

  Application.init();

  Application.willFinishLaunching(() => {
    open Brisk_macos.Menu;
    setServicesMenu(TopMenu.servicesMenu);
    setWindowsMenu(TopMenu.windowsMenu);
    setMainMenu(TopMenu.menu);
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

    let prevToolbar = Toolbar.(ref(<toolbar />));
    let toolbar =
      Toolbar.Reconciler.RenderedElement.render(
        Toolbar.OutputTree.Window(window),
        prevToolbar^,
      )
      |> Toolbar.Reconciler.RenderedElement.executePendingEffects
      |> ref;
    ignore(
      Toolbar.Reconciler.RenderedElement.executeHostViewUpdates(toolbar^),
    );

    let renderToolbar = nextToolbar => {
      toolbar :=
        Toolbar.Reconciler.RenderedElement.update(
          ~previousElement=prevToolbar^,
          nextToolbar,
          ~renderedElement=toolbar^,
        )
        |> Toolbar.Reconciler.RenderedElement.executePendingEffects;
      ignore(
        Toolbar.Reconciler.RenderedElement.executeHostViewUpdates(toolbar^),
      );
      prevToolbar := nextToolbar;
    };

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
      Stories.(<component renderToolbar />),
    );

    RunLoop.spawn();
  });

  Application.run();
};
