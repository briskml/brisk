open Alcotest;

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

let line = (ppf, ~color=?, c) => {
  let line = String.v(~len=terminal_columns, (_) => c);
  switch color {
  | Some(c) => Fmt.pf(ppf, "%a\n%!", Fmt.(styled(c, string)), line)
  | None => Fmt.pf(ppf, "%s\n%!", line)
  };
};

let show_line = msg => {
  line(Fmt.stderr, ~color=`Yellow, '-');
  Printf.eprintf("ASSERT %s\n", msg);
  line(Fmt.stderr, ~color=`Yellow, '-');
};

let check = (t, msg, x, y) => {
  show_line(msg);
  if (! equal(t, x, y)) {
    Fmt.strf(
      "Error %s:\n\nEXPECTED:@\n\n%a\n\nACTUAL:@\n\n%a\n",
      msg,
      pp(t),
      x,
      pp(t),
      y
    )
    |> failwith;
  };
};
