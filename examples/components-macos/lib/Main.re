open Brisk_macos;

let app = {
  open Brisk.Layout;

  let tabs = [Tab.Welcome, Views, Buttons, Text, Image];

  let component = Brisk.component("App");

  (~children as _: list(unit), ()) =>
    component(hooks => {
      let (currentTab, switchTab, hooks) =
        Brisk.Hooks.state(Tab.Welcome, hooks);

      (
        hooks,
        {
          Tabs.(
            <view style=[flex(1.), flexDirection(`Row)]>
              <effectView
                style=EffectView.[blendingMode(`BehindWindow), width(185.)]>
                <view style=[margin2(~v=37., ())]>
                  ...{
                       tabs
                       |> List.map(tab =>
                            <clickable onClick={() => switchTab(tab)}>
                              <text
                                style=[
                                  background(
                                    tab == currentTab
                                      ? Color.hexa("#000000", 0.4)
                                      : Color.transparent,
                                  ),
                                  font(~size=15., ()),
                                  kern(0.09),
                                  align(`Left),
                                  color(Color.hex("#282522")),
                                  padding2(~h=25., ~v=4., ()),
                                ]
                                value={Tab.label(tab)}
                              />
                            </clickable>
                          )
                     }
                </view>
              </effectView>
              <scrollView
                style=[flex(1.), padding4(~top=10., ~left=13., ())]>
                {switch (currentTab) {
                 | Welcome => <welcomeTab />
                 | Views => <viewsTab />
                 | Buttons => <buttonsTab />
                 | Text => <textTab />
                 | Image => <imageTab />
                 | _ => <view />
                 }}
              </scrollView>
            </view>
          );
        },
      );
    });
};
