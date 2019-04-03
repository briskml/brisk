open Brisk_macos.Menu;

let windowsMenu =
  <menu title="Window">
    <item title="Minimize" key="m" action=Minimize />
    <item title="Zoom" action=Zoom />
    <separatorItem />
    <item title="Bring All In Front" action=BringAllInFront />
  </menu>;
let servicesMenu = <menu title="Services" />;

let menu =
  <>
    <menu title="Hacker News">
      <item action=OrderFrontStandardAboutPanel title="About Hacker News" />
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
  </>;
