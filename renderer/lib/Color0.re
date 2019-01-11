type t = Color.Rgba'.t;

exception ParseHexException(string);

let parseExn = (h, c) =>
  switch (c) {
  | Some(v) => v
  | None => raise(ParseHexException("Unable to parse color: " ++ h))
  };

let rgba = (r, g, b, a) => Color.of_rgba(r, g, b, a) |> Color.to_rgba';
let rgb = (r, g, b) => Color.of_rgb(r, g, b) |> Color.to_rgba';

let hsl = (h, s, l) => Color.of_hsl(h, s, l) |> Color.to_rgba';

let hex = c => c |> Color.of_hexstring |> parseExn(c) |> Color.to_rgba';

let alpha: (float, t) => t = (a, c) => {...c, a: a *. c.a};

let hexa = (c, a) => hex(c) |> alpha(a);

let transparent = rgba(0, 0, 0, 0.);

let isTransparent: t => bool =
  ({r, g, b, a}) => 0. == r && 0. == g && 0. == b && 0. == a;
