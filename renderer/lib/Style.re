module Create = (Encoding: Flex.Spec.Encoding) => {
  open Encoding;

  module Position = {
    type position = [ | `absolute | `relative];

    type t = {
      position,
      left: scalar,
      top: scalar,
      right: scalar,
      bottom: scalar,
    };

    let make =
        (
          ~left=cssUndefined,
          ~top=cssUndefined,
          ~right=cssUndefined,
          ~bottom=cssUndefined,
          position,
        ) => {
      let pos: t = {position, left, top, right, bottom};
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
      weight,
      size: scalar,
    };

    let make = (~family="", ~weight=`regular, ~size=cssUndefined, ()) => {
      let font: t = {family, weight, size};
      `font(font);
    };
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

  let width = (w: scalar) => `width(w);
  let height = (h: scalar) => `height(h);

  let background = (color: Color0.t) => `background(color);

  let padding = (p: scalar) => `padding((p, p, p, p));
  let padding2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `padding((h, v, h, v));
  let padding4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `padding((left, top, right, bottom));

  let margin = (m: scalar) => `margin((m, m, m, m));

  let margin2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `margin((h, v, h, v));
  let margin4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `margin((left, top, right, bottom));
};
