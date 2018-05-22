open Alcotest;

let suite = (
  "Term_renderer",
  [
    (
      "Test simple expression",
      `Quick,
      () => check(bool, "It matches", true, 1 == 1),
    ),
  ],
);
