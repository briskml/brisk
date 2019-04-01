open Brisk_macos;


let () = {
  open Cocoa;

  Application.init();

  Application.willFinishLaunching(() => {
    open Brisk_macos.Menu;
    let windowsMenu =
      <menu title="Window">
        <item title="Minimize" key="m" action=Minimize />
        <item title="Zoom" action=Zoom />
        <separatorItem />
        <item title="Bring All In Front" action=BringAllInFront />
      </menu>;
    let servicesMenu = <menu title="Services" />;
    setServicesMenu(servicesMenu);
    setWindowsMenu(windowsMenu);
    setMainMenu(
      <>
        <menu title="Hacker News">
          <item
            action=OrderFrontStandardAboutPanel
            title="About Hacker News"
          />
          <separatorItem />
          <item
            action={
              Callback(
                () =>
                  Brisk_macos.Std.openUrl(
                    "https://github.com/briskml/brisk/tree/master/examples/hacker-news",
                  ),
              )
            }
            title="Show source on GitHub"
          />
          <submenu> ...servicesMenu </submenu>
          <separatorItem />
          <item action=Hide key="h" title="Hide Hacker News" />
          <item
            action=HideOtherApplications
            key="h"
            optionModifier=true
            title="Hide Others"
          />
          <item action=UnhideAllApplications title="Show All" />
          <separatorItem />
          <item action=Terminate key="q" title="Quit Hacker News" />
        </menu>
        <menu title="File">
          <item action=Close key="w" title="Close" />
          <separatorItem />
          <item action=Print key="w" title="Print..." />
        </menu>
        <menu title="Edit">
          <item action=Copy key="c" title="Copy" />
          <item action=Paste key="v" title="Paste" />
        </menu>
        windowsMenu
        <menu title="Help">
          <item action=Help key="?" title="Hacker News Help" />
        </menu>
      </>,
    );
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
