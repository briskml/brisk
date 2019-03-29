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

let timeAgoSince = date => {
  Format.sprintf(
    "%s ago",
    Core_kernel.Time.(diff(now(), date) |> Span.to_string),
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
  Format.sprintf("%s · %s", username, timeAgoSince(time));
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
