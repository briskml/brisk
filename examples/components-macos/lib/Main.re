open Brisk_macos;

type state = {currentTab: Tab.t};

let tabs = [Tab.Welcome, Views, Buttons, Text, Image];

let app = {
  open Brisk.Layout;

  let component = Brisk.component("App");

  (~children as _: list(unit), ()) =>
    component(hooks => {
      let (state, _setState, hooks) =
        Brisk.Hooks.state({currentTab: Views}, hooks);
      (
        hooks,
        {
          open Tabs;

          let {currentTab} = state;

          <view
            style=[
              position(~top=0., ~left=0., ~right=0., ~bottom=0., `Absolute),
              flexDirection(`Row),
            ]>
            <effectView
              style=EffectView.[blendingMode(`BehindWindow), width(185.)]>
              <view style=[margin2(~v=37., ())]>
                ...{
                     tabs
                     |> List.map(tab =>
                          <text
                            style=[
                              background(
                                tab == currentTab
                                  ? Color.hexa("#000000", 0.4)
                                  : Color.transparent,
                              ),
                              ...Theme.sidebarItemStyle,
                            ]
                            value={Tab.label(tab)}
                          />
                        )
                   }
              </view>
            </effectView>
            <scrollView style=[flex(1.), padding4(~top=20., ~left=13., ())]>
              {switch (currentTab) {
               | Welcome => <welcomeTab />
               | Views => <viewsTab />
               | _ => <view />
               }}
            </scrollView>
          </view>;
        },
      );
    });
};
