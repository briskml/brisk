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

let line = (ppf, ~color=?, c) => {
  open Astring;
  let with_process_in = (cmd, f) => {
    let ic = Unix.open_process_in(cmd);
    try {
      let r = f(ic);
      ignore(Unix.close_process_in(ic));
      r;
    } {
    | exn =>
      ignore(Unix.close_process_in(ic));
      raise(exn);
    };
  };
  let terminal_columns =
    /* terminfo */
    try (with_process_in("tput cols", ic => int_of_string(input_line(ic)))) {
    | _ =>
      /* GNU stty */
      try (
        with_process_in("stty size", ic =>
          switch (String.cuts(input_line(ic), ~sep=" ")) {
          | [_, v] => int_of_string(v)
          | _ => failwith("stty")
          }
        )
      ) {
      | _ =>
        /* shell envvar */
        try (int_of_string(Sys.getenv("COLUMNS"))) {
        | _ =>
          /* default */
          80
        }
      }
    };
  let line = String.v(~len=terminal_columns, (_) => c);
  switch color {
  | Some(c) => Fmt.pf(ppf, "%a\n%!", Fmt.(styled(c, string)), line)
  | None => Fmt.pf(ppf, "%s\n%!", line)
  };
};

let check = (t, msg, x, y) =>
  if (! equal(t, x, y)) {
    line(Fmt.stderr, ~color=`Yellow, '-');
    Fmt.strf(
      "%s:\n\nEXPECTED:@\n\n%a\n\nACTUAL:@\n\n%a\n",
      msg,
      pp(t),
      x,
      pp(t),
      y
    )
    |> failwith;
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
