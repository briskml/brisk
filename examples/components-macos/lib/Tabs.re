open Brisk_macos;

let hairline = {
  let component = Brisk.component("HairlineView");

  (~style=[], ~children as _: list(unit), ()) =>
    component(hooks =>
      Brisk.Layout.(
        hooks,
        {
          <view
            style=[background(Color.hex("#e3e3e3")), height(1.), ...style]
          />;
        },
      )
    );
};

let section = {
  let component = Brisk.component("Section");

  (~title, ~style=[], ~children: list(Brisk.syntheticElement), ()) =>
    component(hooks =>
      (
        hooks,
        {
          <view style>
            <text style=Theme.sectionHeaderStyle value=title />
            {Brisk.listToElement(children)}
          </view>;
        },
      )
    );
};

let welcomeTab = {
  let component = Brisk.component("WelcomeTab");

  (~children as _: list(unit), ()) =>
    component(hooks => {
      open Brisk.Layout;

      let (_state, setState, hooks) = Brisk.Hooks.state(None, hooks);

      (
        hooks,
        {
          <view style=[height(600.), padding4(~top=20., ~left=17., ())]>
            <text style=Theme.headerStyle value="Welcome to Brisk" />
            <view style=[justifyContent(`Center), alignContent(`Center)]>
              <text
                style=[
                  font(~size=18., ()),
                  align(`Center),
                  alignSelf(`Center),
                  width(200.),
                  border(~radius=10., ()),
                  color(Color.hex("#ffffff")),
                  background(Color.hexa("#263ac5", 0.9)),
                  margin(20.),
                  padding2(~h=10., ~v=10., ()),
                ]
                value="Text bubble"
              />
            </view>
            <button
              style=[
                width(400.),
                height(60.),
                margin4(~top=20., ()),
                alignSelf(`Center),
                font(~size=16., ()),
                color(Color.hex("#ffffff")),
                background(Color.hex("#263ac5")),
                align(`Center),
              ]
              title="You're gonna have to wait 1 second"
              callback={() =>
                Lwt.Infix.(
                  ignore(
                    Lwt_unix.sleep(1.)
                    >>= (_ => Lwt.return(setState(Some(100)))),
                  )
                )
              }
            />
            <view style=[alignContent(`Center), height(600.)]>
              <text
                style=[
                  font(~size=18., ()),
                  align(`Center),
                  alignSelf(`Center),
                  width(200.),
                  height(300.),
                  border(~radius=10., ()),
                  color(Color.hex("#011021")),
                  margin(20.),
                  padding2(~h=10., ~v=10., ()),
                ]
                value="Very large height for scrolling"
              />
            </view>
          </view>;
        },
      );
    });
};

let viewsTab = {
  open Brisk.Layout;

  let component = Brisk.component("ViewsTab");

  (~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        {
          <view style=[padding4(~top=10., ~left=17., ~bottom=40., ())]>
            <text style=Theme.headerStyle value="View Styles" />
            <hairline />
            <section title="Border and rounded corners">
              <view
                style=[
                  width(194.),
                  height(120.),
                  border(
                    ~width=1.,
                    ~radius=14.,
                    ~color=Color.hex("#979797"),
                    (),
                  ),
                  background(Color.hex("#d8d8d8")),
                ]
              />
            </section>
            <section title="Window background blur">
              <effectView
                style=EffectView.[
                  width(194.),
                  height(120.),
                  border(~radius=14., ()),
                  blendingMode(`BehindWindow),
                ]
              />
            </section>
            <section title="Blur">
              <view
                style=[
                  /* TODO Add clipBackground property */
                  width(194.),
                  height(120.),
                  border(~radius=14., ()),
                  flexDirection(`Row),
                ]>
                <view style=[flex(1.), background(Color.hex("#fff"))] />
                <view style=[flex(1.), background(Color.hex("#000"))] />
                <effectView
                  style=EffectView.[
                    position(
                      ~top=0.,
                      ~bottom=0.,
                      ~left=0.,
                      ~right=0.,
                      `Absolute,
                    ),
                    blendingMode(`WithinWindow),
                  ]
                />
              </view>
            </section>
            <section title="Shadow">
              <view
                style=[
                  width(194.),
                  height(120.),
                  border(~radius=14., ()),
                  shadow(
                    ~blur=10.,
                    ~color=Color.hex("#000000"),
                    ~opacity=0.29,
                    (),
                  ),
                  background(Color.hex("#d8d8d8")),
                ]
              />
            </section>
          </view>;
        },
      )
    );
};
