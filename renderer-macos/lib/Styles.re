open CocoaTypes;
open Brisk.Layout;
open BriskStylableText;

type viewStyle = [
  | `Border(Border.t)
  | `Overflow(FlexLayout.LayoutSupport.LayoutTypes.overflow)
  | `Shadow(Shadow.t)
  | `Background(Color.t)
];

type textStyle = [
  | `Color(Color.t)
  | `Font(Font.t)
  | `Kern(float)
  | `Align(Alignment.t)
  | `LineBreak(LineBreak.t)
  | `LineSpacing(float)
];

let commitTextStyle = BriskStylableText.applyChanges;

let setViewStyle = (view: view, attribute: [> viewStyle]) =>
  switch (attribute) {
  | `Background(({r, g, b, a}: Color.t)) =>
    BriskView.setBackgroundColor(view, r, g, b, a)
  | `Border({Border.width, radius, color}) =>
    if (!isUndefined(width)) {
      BriskView.setBorderWidth(view, width);
    };
    if (!isUndefined(radius)) {
      BriskView.setBorderRadius(view, radius);
    };

    if (color !== Color.undefined) {
      let {r, g, b, a}: Color.t = color;
      BriskView.setBorderColor(view, r, g, b, a);
    };
  | `Shadow({Shadow.x, y, opacity, blur, color}) =>
    let x = isUndefined(x) ? 0. : x;
    let y = isUndefined(y) ? 0. : y;

    BriskView.setShadowOffset(view, x, y);

    if (!isUndefined(opacity)) {
      BriskView.setShadowOpacity(view, opacity);
    };
    if (!isUndefined(blur)) {
      BriskView.setShadowRadius(view, blur);
    };

    if (color !== Color.undefined) {
      let {r, g, b, a}: Color.t = color;
      BriskView.setShadowColor(view, r, g, b, a);
    };
  | `Overflow(overflow) =>
    BriskView.setMasksToBounds(view, overflow == Hidden)
  | _ => ()
  };

let setTextStyle = (txt: text, attribute: [> textStyle]) =>
  switch (attribute) {
  | `Font({Font.family, size, weight}) =>
    let weight =
      switch (weight) {
      | `UltraLight => (-0.8)
      | `Thin => (-0.6)
      | `Light => (-0.4)
      | `Regular => 0.
      | `Medium => 0.23
      | `Semibold => 0.3
      | `Bold => 0.4
      | `Heavy => 0.56
      | `Black => 0.62
      };
    setFont(txt, family, size, weight);
  | `Kern(kern) => setKern(txt, kern)
  | `Align(align) =>
    setAlignment(
      txt,
      switch (align) {
      | `Left => 0
      | `Right => 1
      | `Center => 2
      | `Justified => 3
      | `Natural => 4
      },
    )
  | `LineBreak(mode) =>
    setLineBreak(
      txt,
      switch (mode) {
      | `WordWrap => 0
      | `CharWrap => 1
      | `Clip => 2
      | `TruncateHead => 3
      | `TruncateTale => 4
      | `TruncateMiddle => 5
      },
    )
  | `LineSpacing(spacing) => setLineSpacing(txt, spacing)
  | `Color(({r, g, b, a}: Color.t)) => setColor(txt, r, g, b, a)
  | _ => ()
  };
