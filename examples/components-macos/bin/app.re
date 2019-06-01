open Brisk_macos;
open Components_macos;

module UIRunLoop = RunLoop.Make(UIRunner);

let () = {
  open Cocoa;

  let appName = "BriskMac";

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
        <menu title="File"> <item action=Close key="w" title="Close" /> </menu>
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
        ~title=appName,
        ~contentView=view,
        ~contentIsFullSize=true,
        ~onResize=
          window => UIRunner.setWindowHeight(Window.contentHeight(window)),
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
          {view, isYAxisFlipped: false},
        );
      {Brisk.OutputTree.view, layoutNode};
    };

    UIRunner.setWindowHeight(Window.contentHeight(window));

    UIRunLoop.renderAndMount(root, Main.(<app />));

    UIRunLoop.spawn();
  });

  Application.run();
};
