open Alcotest;

let suites = [
  (
    "Test simple expression",
    `Quick,
    () => check(bool, "It matches", true, 1 == 1),
  ),
];

/** Annoying dune progress */
print_endline("");

Alcotest.run(
  ~argv=[|"--verbose --color"|],
  "Brisk_term",
  [("Term_renderer", suites)],
);
