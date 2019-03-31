type by = {id: string};

type kid = {
  text: string,
  by,
  time: int,
};

type kids = array(kid);

type story = {
  title: string,
  url: option(string),
  score: int,
  by,
  time: int,
  descendants: int,
  kids: option(kids),
};

type storyWithRelativeTime = {
  story,
  timeAgo: string,
};

type poll = {title: string};
type job = poll;

let unitOfTimeToString =
  fun
  | Core_kernel.Unit_of_time.Nanosecond => "nanosecond"
  | Microsecond => "microsecond"
  | Millisecond => "millisecond"
  | Second => "second"
  | Minute => "minute"
  | Hour => "hour"
  | Day => "day";

let getTimeComponent = {
  Core_kernel.Time.Span.(
    fun
    | Core_kernel.Unit_of_time.Nanosecond => to_ns
    | Microsecond => to_us
    | Millisecond => to_ms
    | Second => to_sec
    | Minute => to_min
    | Hour => to_hr
    | Day => to_day
  );
};

let timeAgoSinceNow = date => {
  let span = Core_kernel.Time.(diff(now(), date));
  let unitOfTime = Core_kernel.Time.Span.to_unit_of_time(span);
  let mostSignificantTimeComponent =
    getTimeComponent(unitOfTime, span) |> int_of_float;
  let unitOfTimeString = unitOfTimeToString(unitOfTime);
  Printf.sprintf(
    "%i %s ago",
    mostSignificantTimeComponent,
    mostSignificantTimeComponent > 1
      ? unitOfTimeString ++ "s" : unitOfTimeString,
  );
};

let formatUrl = {
  let regex = Re.seq([Re.start, Re.str("www.")]) |> Re.compile;
  fun
  | Some(url) => {
      switch (Uri.host(Uri.of_string(url))) {
      | Some(url) when Re.execp(regex, url) =>
        Some(String.sub(url, 4, String.length(url) - 4))
      | Some(url) => Some(url)
      | None => None
      };
    }
  | None => None;
};

let formatComment = txt => {
  open Markup;
  /***
   * TODO: We need to process signals stream and
   * replace all <p> tags with a couple of \n\n
   */
  let transcoded =
    txt |> string |> parse_html |> signals |> write_html |> to_string;

  transcoded;
};

let formatCommentDetails = (~username, ~time) => {
  let time = Core_kernel.Time.(of_span_since_epoch(Span.of_int_sec(time)));
  Format.sprintf("%s · %s", username, timeAgoSinceNow(time));
};

let formatDetails = (~username, ~score, ~commentCount, ~url) => {
  let commonDetails =
    Format.sprintf(
      "%i points | %i comments | by %s",
      score,
      commentCount,
      username,
    );

  switch (formatUrl(url)) {
  | Some(str) => Format.sprintf("%s | source: %s", commonDetails, str)

  | None => commonDetails
  };
};
