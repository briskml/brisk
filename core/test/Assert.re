open Alcotest;
open TestReactCore;

type mountElement = RenderedElement.t;

type mount = list(Implementation.testMountEntry);

type testHostItem('a) =
  | MountElement(mountElement): testHostItem(mount);

let mountLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printMountLog(formatter, t),
    Implementation.equal_testMountLog,
  );

let diffOutput = (expected, actual) => {
  let gray = string => "\027[90m" ++ string ++ "\027[39m";
  let red = string => "\027[31m" ++ string ++ "\027[39m";
  let green = string => "\027[32m" ++ string ++ "\027[39m";
  module Diff = Patience_diff_lib.Patience_diff;
  module StringDiff = Patience_diff_lib.Patience_diff.String;
  let diff =
    StringDiff.get_hunks(
      ~transform=x => x,
      ~big_enough=?None,
      ~context=-1,
      ~mine=expected,
      ~other=actual,
    );
  diff
  |> Diff.Hunks.ranges
  |> List.map(
       fun
       | Diff.Range.Same(lines) =>
         gray(
           String.concat(
             "\n",
             Array.map(((line, _)) => " " ++ line, lines) |> Array.to_list,
           ),
         )
       | Old(lines) =>
         red(
           String.concat(
             "\n",
             lines |> Array.map(line => "-" ++ line) |> Array.to_list,
           ),
         )
       | New(lines) =>
         green(
           String.concat(
             "\n",
             lines |> Array.map(line => "+" ++ line) |> Array.to_list,
           ),
         )
       | Replace(expected, actual) =>
         red(
           String.concat(
             "\n",
             actual |> Array.map(line => "-" ++ line) |> Array.to_list,
           ),
         )
         ++ "\n"
         ++ green(
              String.concat(
                "\n",
                expected |> Array.map(line => "+" ++ line) |> Array.to_list,
              ),
            )
       | Unified(_) => failwith("UNEXPECTED FAILURE"),
     )
  |> String.concat("\n");
};

let line = (ppf, c) => {
  let line = Astring.String.v(~len=80, _ => c);
  Fmt.pf(ppf, "%a\n%!", Fmt.(styled(`Yellow, string)), line);
};

let check = (t, msg, x, y) =>
  if (!equal(t, x, y)) {
    line(Fmt.stderr, '-');
    let expected =
      Fmt.strf("%a", pp(t), x)
      |> String.split_on_char('\n')
      |> Array.of_list;
    let actual =
      Fmt.strf("%a", pp(t), y)
      |> String.split_on_char('\n')
      |> Array.of_list;
    let diff = diffOutput(expected, actual);
    Fmt.strf("%s:\n\n%s\n", msg, diff) |> failwith;
  };

let assertMountLog = (~label="", expected, actual) => {
  Implementation.mountLog := [];
  check(mountLog, label, expected, List.rev(actual));
};

type testState = {
  syntheticElement,
  renderedElement: RenderedElement.t,
};

let render = (root, syntheticElement) => {
  syntheticElement,
  renderedElement: RenderedElement.render(root, syntheticElement),
};

let reset = x => {
  Implementation.mountLog := [];
  x;
};

let update =
    (nextReactElement, {syntheticElement: previousElement, renderedElement}) => {
  syntheticElement: nextReactElement,
  renderedElement:
    RenderedElement.update(
      ~previousElement,
      ~renderedElement,
      nextReactElement,
    ),
};

let flushPendingUpdates = ({renderedElement, syntheticElement}) => {
  syntheticElement,
  renderedElement: RenderedElement.flushPendingUpdates(renderedElement),
};

let executeSideEffects = ({renderedElement} as testState) => {
  RenderedElement.executeHostViewUpdates(renderedElement) |> ignore;
  List.iter(f => f(), renderedElement.enqueuedEffects);
  testState;
};

let expect = (~label=?, expected, testState) => {
  let mountLog = Implementation.mountLog^;
  assertMountLog(~label?, expected, mountLog);
  reset(testState);
};

let act = (~action, rAction, testState) => {
  RemoteAction.send(rAction, ~action);
  testState;
};
