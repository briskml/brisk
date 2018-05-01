open Alcotest;

let renderedElement =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printElement(formatter, t),
    TestRenderer.compareElement
  );

let topLevelUpdateLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printTopLevelUpdateLog(formatter, t),
    TestRenderer.compareTopLevelUpdateLog
  );

let updateLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printUpdateLog(formatter, t),
    TestRenderer.compareUpdateLog
  );

module Diff = Simple_diff.Make(String);

let diffOutput = (expected, actual) => {
  open Diff;
  let gray = string => "\027[90m" ++ string ++ "\027[39m";
  let red = string => "\027[31m" ++ string ++ "\027[39m";
  let green = string => "\027[32m" ++ string ++ "\027[39m";
  let diff = get_diff(expected, actual);
  diff
  |> List.map(
       fun
       | Equal(lines) =>
         gray(
           String.concat(
             "\n",
             lines |> Array.map(line => " " ++ line) |> Array.to_list
           )
         )
       | Deleted(lines) =>
         red(
           String.concat(
             "\n",
             lines |> Array.map(line => "-" ++ line) |> Array.to_list
           )
         )
       | Added(lines) =>
         green(
           String.concat(
             "\n",
             lines |> Array.map(line => "+" ++ line) |> Array.to_list
           )
         )
     )
  |> String.concat("\n");
};

let line = (ppf, c) => {
  let line = Astring.String.v(~len=80, (_) => c);
  Fmt.pf(ppf, "%a\n%!", Fmt.(styled(`Yellow, string)), line);
};

let check = (t, msg, x, y) =>
  if (! equal(t, x, y)) {
    line(Fmt.stderr, '-');
    let expected =
      Fmt.strf("%a", pp(t), x) |> String.split_on_char('\n') |> Array.of_list;
    let actual =
      Fmt.strf("%a", pp(t), y) |> String.split_on_char('\n') |> Array.of_list;
    let diff = diffOutput(expected, actual);
    Fmt.strf("%s:\n\n%s\n", msg, diff) |> failwith;
  };

let assertElement = (~label="", expected, rendered) =>
  check(
    renderedElement,
    label,
    expected,
    TestRenderer.convertElement(rendered)
  );

let assertUpdateLog = (~label="", expected, actual) =>
  check(updateLog, label, expected, TestRenderer.convertUpdateLog(actual));

let assertTopLevelUpdateLog = (~label="", expected, actual) =>
  check(
    topLevelUpdateLog,
    label,
    expected,
    TestRenderer.convertTopLevelUpdateLog(actual)
  );

let assertFlushUpdate =
    (~label="", (expectedElement, expectedLog), (actualElement, actualLog)) => {
  assertElement(~label, expectedElement, actualElement);
  assertUpdateLog(~label, expectedLog, actualLog);
};

let assertUpdate =
    (~label="", (expectedElement, expectedLog), (actualElement, actualLog)) => {
  assertElement(~label, expectedElement, actualElement);
  assertTopLevelUpdateLog(~label, expectedLog, actualLog);
};
