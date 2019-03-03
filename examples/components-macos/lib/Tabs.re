open Brisk_macos;

let hairline = (~style=[], ~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[background(Color.hex("#e3e3e3")), height(1.), ...style] />
  );
};

let section = (~style=[], ~title, ~children, ()) =>
  <view style>
    <text style=Theme.sectionHeaderStyle value=title />
    {Brisk.listToElement(children)}
  </view>;

let welcomeTab = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[padding4(~bottom=40., ())]>
      <text style=Theme.headerStyle value="Welcome" />
      <hairline />
      <view style=[margin2(~v=20., ()), ...Theme.sectionStyle]>
        <image
          style=[width(224.), height(53.)]
          source={`Bundle("brisk")}
        />
      </view>
    </view>
  );
};

let viewsTab = {
  open Brisk.Layout;

  let component = Brisk.component("ViewsTab");

  (~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        {
          <view style=[padding4(~bottom=40., ())]>
            <text style=Theme.headerStyle value="View Styles" />
            <hairline />
            <section
              style=Theme.sectionStyle title="Border and rounded corners">
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
                  width(194.),
                  height(120.),
                  overflow(`Hidden),
                  border(~radius=14., ()),
                  flexDirection(`Row),
                ]>
                <view style=[flex(1.), background(Color.hex("#ff9999"))] />
                <view style=[flex(1.), background(Color.hex("#bd10e0"))] />
                <effectView
                  style=EffectView.[
                    position(
                      ~top=0.,
                      ~bottom=0.,
                      ~left=0.,
                      ~right=0.,
                      `Absolute,
                    ),
                    border(~radius=14., ()),
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

let buttonsTab = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[padding4(~bottom=40., ())]>
      <text style=Theme.headerStyle value="Buttons" />
      <hairline />
      <section style=Theme.sectionStyle title="Styled">
        <text style=Theme.sectionContent value="TODO: Background and sizes" />
      </section>
      <section style=Theme.sectionStyle title="System">
        <text
          style=Theme.sectionContent
          value="TODO: NSButton styles lineup (bezelStyle, buttonType)"
        />
      </section>
    </view>
  );
};

let textTab = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[padding4(~bottom=40., ())]>
      <text style=Theme.headerStyle value="Text" />
      <hairline />
      <section style=Theme.sectionStyle title="Styled">
        <text style=Theme.sectionContent value="TODO: Font, color" />
      </section>
    </view>
  );
};

let imageTab = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[padding4(~bottom=40., ())]>
      <text style=Theme.headerStyle value="Image" />
      <hairline />
      <section style=Theme.sectionStyle title="Bundle">
        <text style=Theme.sectionContent value="TODO: Bundled asset images" />
      </section>
      <section style=Theme.sectionStyle title="Files">
        <text style=Theme.sectionContent value="TODO: Load from file" />
      </section>
      <section style=Theme.sectionStyle title="Remote">
        <text style=Theme.sectionContent value="TODO: Fetch from URL" />
      </section>
    </view>
  );
};
