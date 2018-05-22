/** Annoying dune progress */
print_endline("");

Alcotest.run(
  ~argv=[|"--verbose --color"|],
  "Brisk",
  [Test_core.Test.suite, Test_term.Test.suite],
);
