module Create = (Encoding: Flex.Spec.Encoding) => {
  open Encoding;

  type inset = {
    left: scalar,
    top: scalar,
    right: scalar,
    bottom: scalar,
  };

  module Position = {
    type position = [ | `Absolute | `Relative];

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
      `Position(pos);
    };
  };

  module Font = {
    type weight = [
      | `UltraLight
      | `Thin
      | `Light
      | `Regular
      | `Medium
      | `Semibold
      | `Bold
      | `Heavy
      | `Black
    ];

    type t = {
      family: string,
      size: scalar,
      weight,
    };

    let make = (~family="", ~size=cssUndefined, ~weight=`Regular, ()) => {
      let font: t = {family, size, weight};
      `Font(font);
    };
  };

  module Alignment = {
    type t = [ | `Left | `Right | `Center | `Justified | `Natural];

    let make = (alignment: t) => `Align(alignment);
  };

  module LineBreak = {
    type t = [
      | `WordWrap
      | `CharWrap
      | `Clip
      | `TruncateHead
      | `TruncateTale
      | `TruncateMiddle
    ];

    let make = (mode: t) => `LineBreak(mode);
  };

  module Border = {
    type t = {
      width: scalar,
      radius: scalar,
      color: Color0.t,
    };

    let make =
        (
          ~width=cssUndefined,
          ~radius=cssUndefined,
          ~color=Color0.undefined,
          (),
        ) => {
      let border: t = {width, radius, color};
      `Border(border);
    };

    let width = (width: Encoding.scalar) => make(~width, ());
    let color = (color: Color0.t) => make(~color, ());
  };

  let position = Position.make;
  let border = Border.make;
  let font = Font.make;
  let kern = f => `Kern(f);
  let align = Alignment.make;
  let lineBreak = LineBreak.make;
  let lineSpacing = f => `LineSpacing(f);

  let width = (w: scalar) => `Width(w);
  let height = (h: scalar) => `Height(h);

  let color = (color: Color0.t) => `Color(color);
  let background = (color: Color0.t) => `Background(color);

  let padding = (p: scalar) =>
    `Padding({left: p, top: p, right: p, bottom: p});
  let padding2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `Padding({left: h, top: v, right: h, bottom: v});
  let padding4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `Padding({left, top, right, bottom});

  let margin = (m: scalar) =>
    `Margin({left: m, top: m, right: m, bottom: m});

  let margin2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `Margin({left: h, top: v, right: h, bottom: v});
  let margin4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `Margin({left, top, right, bottom});
};
