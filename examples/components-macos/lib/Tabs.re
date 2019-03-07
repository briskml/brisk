open Brisk_macos;

module Theme = {
  open Brisk.Layout;

  let headerStyle = [
    font(~size=24., ~weight=`Semibold, ()),
    kern(0.58),
    align(`Left),
    color(Color.hex("#000000")),
    padding4(~left=4., ~top=0., ()),
    margin2(~v=10., ()),
  ];

  let sectionStyle = [padding4(~left=4., ())];

  let sectionHeaderStyle = [
    font(~size=17., ()),
    kern(0.27),
    color(Color.hex("#000000")),
    margin2(~v=9., ()),
  ];

  let sectionContent = [font(~size=15., ()), color(Color.hex("#000000"))];
};

let hairline = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[background(Color.hex("#e3e3e3")), height(1.)] />
  );
};

let section = (~title, ~children, ()) =>
  <view style=Theme.sectionStyle>
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
  Brisk.Layout.(
    (~children as _: list(unit), ()) =>
      <view style=[padding4(~bottom=40., ())]>
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
                position(~top=0., ~bottom=0., ~left=0., ~right=0., `Absolute),
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
      </view>
  );
};

let buttonsTab = (~children as _: list(unit), ()) => {
  Brisk.Layout.(
    <view style=[padding4(~bottom=40., ())]>
      <text style=Theme.headerStyle value="Buttons" />
      <hairline />
      <section title="Styled">
        <text style=Theme.sectionContent value="TODO: Background and sizes" />
      </section>
      <section title="System">
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
      <section title="Styled">
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
      <section title="Bundle">
        <text style=Theme.sectionContent value="TODO: Bundled asset images" />
      </section>
      <section title="Files">
        <text style=Theme.sectionContent value="TODO: Load from file" />
      </section>
      <section title="Remote">
        <text style=Theme.sectionContent value="TODO: Fetch from URL" />
      </section>
    </view>
  );
};
