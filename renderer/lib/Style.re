module Create = (Encoding: Flex.Spec.Encoding) => {
  open Encoding;

  type inset = {
    left: scalar,
    top: scalar,
    right: scalar,
    bottom: scalar,
  };

  module Position = {
    type position = [ | `absolute | `relative];

    type t = {
      position,
      inset,
    };

    let make =
        (
          ~left=cssUndefined,
          ~top=cssUndefined,
          ~right=cssUndefined,
          ~bottom=cssUndefined,
          position,
        ) => {
      let pos: t = {
        position,
        inset: {
          left,
          top,
          right,
          bottom,
        },
      };
      `position(pos);
    };
  };

  module Font = {
    type weight = [
      | `ultraLight
      | `thin
      | `light
      | `regular
      | `medium
      | `semibold
      | `bold
      | `heavy
      | `black
    ];

    type t = {
      family: string,
      size: scalar,
      weight,
    };

    let make = (~family="", ~size=cssUndefined, ~weight=`regular, ()) => {
      let font: t = {family, size, weight};
      `font(font);
    };
  };

  module Alignment = {
    type t = [ | `left | `right | `center | `justified | `natural];

    let make = (alignment: t) => `align(alignment);
  };

  module LineBreak = {
    type t = [
      | `wordWrap
      | `charWrap
      | `clip
      | `truncateHead
      | `truncateTale
      | `truncateMiddle
    ];

    let make = (mode: t) => `lineBreak(mode);
  };

  module Border = {
    type t = {
      width: scalar,
      color: Color0.t,
    };

    let make = (~width=cssUndefined, ~color=Color0.transparent, ()) => {
      let border: t = {width, color};
      `border(border);
    };

    let width = (width: Encoding.scalar) => make(~width, ());
    let color = (color: Color0.t) => make(~color, ());
  };

  let position = Position.make;
  let border = Border.make;
  let font = Font.make;
  let kern = f => `kern(f);
  let align = Alignment.make;
  let lineBreak = LineBreak.make;
  let lineSpacing = f => `lineSpacing(f);
  let cornerRadius = f => `cornerRadius(f);

  let width = (w: scalar) => `width(w);
  let height = (h: scalar) => `height(h);

  let color = (color: Color0.t) => `color(color);
  let background = (color: Color0.t) => `background(color);

  let padding = (p: scalar) =>
    `padding({left: p, top: p, right: p, bottom: p});
  let padding2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `padding({left: h, top: v, right: h, bottom: v});
  let padding4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `padding({left, top, right, bottom});

  let margin = (m: scalar) =>
    `margin({left: m, top: m, right: m, bottom: m});

  let margin2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `margin({left: h, top: v, right: h, bottom: v});
  let margin4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `margin({left, top, right, bottom});
};
