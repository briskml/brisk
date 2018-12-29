open Alcotest;
open TestReactCore;

/*
type updateState = {
  previousReactElement: reactElement,
  oldRenderedElement: renderedElement,
  nextReactElement: reactElement,
};

type update = (TestRenderer.t, option(TestRenderer.testTopLevelUpdateLog));

type flushUpdates = (TestRenderer.t, list(TestRenderer.testUpdateEntry));

type testItem('a) =
  | FirstRender(reactElement): testItem(TestRenderer.t)
  | Update(updateState): testItem(update)
  | FlushUpdates(reactElement, renderedElement)
    : testItem(option(flushUpdates));

*/
type mountElement = RenderedElement.t;

type mount = list(Implementation.testMountEntry);
 
type testHostItem('a) =
  | MountElement(mountElement): testHostItem(mount);
/*

let renderedElement =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printElement(formatter, t),
    TestRenderer.equal,
  );

let topLevelUpdateLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printTopLevelUpdateLog(formatter, t),
    TestRenderer.equal_optionTestTopLevelUpdateLog,
  );
  */

let mountLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printMountLog(formatter, t),
    Implementation.equal_testMountLog,
  );

/*
let updateLog =
  Alcotest.testable(
    (formatter, t) => TestPrinter.printUpdateLog(formatter, t),
    TestRenderer.equal_testUpdateLog,
  );
*/

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
             lines |> Array.map(line => " " ++ line) |> Array.to_list,
           ),
         )
       | Deleted(lines) =>
         red(
           String.concat(
             "\n",
             lines |> Array.map(line => "-" ++ line) |> Array.to_list,
           ),
         )
       | Added(lines) =>
         green(
           String.concat(
             "\n",
             lines |> Array.map(line => "+" ++ line) |> Array.to_list,
           ),
         ),
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
/*

let assertElement = (~label="", expected, rendered) =>
  check(
    renderedElement,
    label,
    expected,
    TestRenderer.convertElement(rendered),
  );
*/

let assertMountLog = (~label="", expected, actual) => {
  Implementation.mountLog := [];
  check(mountLog, label, expected, List.rev(actual));
};
/*

let assertUpdateLog = (~label="", expected, actual) =>
  check(updateLog, label, expected, TestRenderer.convertUpdateLog(actual));

let assertTopLevelUpdateLog = (~label="", expected, actual) =>
  check(
    topLevelUpdateLog,
    label,
    expected,
    TestRenderer.convertTopLevelUpdateLog(actual),
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
  */

let expectHost: type a. (~label: string=?, a, testHostItem(a)) => a =
  (~label=?, expected, prev) =>
    switch (prev) {
    | MountElement(renderedElement) =>
      HostView.mountRenderedElement(renderedElement);
      let mountLog = Implementation.mountLog^;
      assertMountLog(~label?, expected, mountLog);
      mountLog;
    };

/*
let expect:
  type a.
    (~label: string=?, a, testItem(a)) => (RenderedElement.t, reactElement) =
  (~label=?, expected, prev) =>
    switch (prev) {
    | Update({nextReactElement, oldRenderedElement, previousReactElement}) =>
      let (newRenderedElement, _) as actual =
        RenderedElement.update(
          ~previousReactElement,
          ~renderedElement=oldRenderedElement,
          nextReactElement,
        );
      assertUpdate(~label?, expected, actual);
      (newRenderedElement, nextReactElement);
    | FirstRender(previousReactElement) =>
      open TestRenderer;
      let oldRenderedElement = render(previousReactElement);
      assertElement(~label?, expected, oldRenderedElement);
      (oldRenderedElement, previousReactElement);
    | FlushUpdates(previousReactElement, oldRenderedElement) =>
      let (newRenderedElement, _) as actual =
        RenderedElement.flushPendingUpdates(oldRenderedElement);
      switch (expected) {
      | Some(expected) =>
        assertFlushUpdate(~label?, expected, actual);
        (newRenderedElement, previousReactElement);
      | None =>
        check(
          bool,
          switch (label) {
          | None => "It is memoized"
          | Some(x) => x
          },
          oldRenderedElement === newRenderedElement,
          true,
        );
        (newRenderedElement, previousReactElement);
      };
    };

let start = reactElement => {
  GlobalState.reset();
  Hashtbl.clear(Implementation.map);
  FirstRender(reactElement);
};

let act = (~action, rAction, (oldRenderedElement, previousReactElement)) => {
  RemoteAction.act(rAction, ~action);
  (oldRenderedElement, previousReactElement);
};
*/

let mount = (renderedElement) =>
  MountElement(renderedElement);

/*
let update = (nextReactElement, (oldRenderedElement, previousReactElement)) =>
  Update({nextReactElement, oldRenderedElement, previousReactElement});

let flushPendingUpdates = ((oldRenderedElement, previousReactElement)) =>
  FlushUpdates(previousReactElement, oldRenderedElement);
  */
