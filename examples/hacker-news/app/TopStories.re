type by = {id: string};

type story = {
  title: string,
  url: option(string),
  score: int,
  by,
  time: int,
  descendants: int,
};

type storyWithRelativeTime = {
  story,
  timeAgo: string,
};

type poll = {title: string};
type job = poll;

type hn = {
  topStories: array([ | `Poll(poll) | `Story(story) | `Job(job)]),
};

type stories = {hn: option(hn)};

module Query = [%graphql
  {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   topStories(offset: $offset, limit: $limit) {
     ... on Story @bsRecord {
       title
       url
       score
       by @bsRecord {
	      id
       }
       time,
  descendants
     }
     ... on Poll @bsRecord {
       title
     }
     ... on Job @bsRecord {
       title
     }
   }
  }
}|}
];

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

let story =
    (~story as {story, timeAgo}, ~index, ~children as _: list(unit), ()) =>
  Brisk_macos.(
    Brisk.Layout.(
      <view style=[height(47.), background(Color.hex("#FFFFFF"))]>
        <view
          style=[
            alignItems(`Center),
            flexDirection(`Row),
          ]>
          <text
            style=[
              height(20.),
              width(30.),
              align(`Center),
              font(~size=13., ()),
              color(Color.hex("#B0B0B0")),
              /* Fixme, cannot fix any bc there'll be an exception */
            ]
            value={string_of_int(index)}
          />
          <view style=[Brisk.Layout.flex(1.)]>
            <view style=Brisk.Layout.[flexDirection(`Row)]>
              <text
                style=[
                  flex(1.),
                  font(~size=13., ()),
                  lineBreak(`TruncateTail),
                  border(~width=1., ~color=Color.hex("#000000"), ()),
                  color(Color.hex("#000000")),
                ]
                value={story.title}
              />
            </view>
            <text
              style=Brisk.Layout.[color(Color.hex("#888888")),
                  border(~width=1., ~color=Color.hex("#000000"), ()),
              
              ]
              value={formatDetails(
                ~username=story.by.id,
                ~score=story.score,
                ~commentCount=story.descendants,
                ~url=story.url,
              )}
            />
          </view>
          <text value=timeAgo />
        </view>
      </view>
    )
  );

type loadingState('a) =
  | Loading
  | Error(GraphQLClient.Error.t, list(list('a)))
  | Ok(list(list('a)));

let getResultList =
  fun
  | Loading => []
  | Error(_, l) => l
  | Ok(l) => l;

let paging = (makeRequest, pageSize, hooks) => {
  open Brisk_macos.Brisk;
  let (loadNextPageAction, _, hooks) =
    Hooks.state(RemoteAction.create(), hooks);
  let (offset, setOffset, hooks) = Hooks.state(0, hooks);
  let (results, setResults, hooks) = Hooks.state(Loading, hooks);
  let loadNext = () => {
    makeRequest(~offset, ~pageSize)
    |> Lwt.map(
         fun
         | Result.Ok(next) => {
             setResults(Ok([next, ...getResultList(results)]));
             setOffset(offset + pageSize);
           }
         | Result.Error(err) => {
             setResults(Error(err, getResultList(results)));
           },
       )
    |> ignore;
  };
  let hooks =
    Hooks.effect(
      OnMount,
      () => {
        loadNext();
        None;
      },
      hooks,
    );
  let hooks =
    Hooks.effect(
      Always,
      () =>
        Some(RemoteAction.subscribe(~handler=loadNext, loadNextPageAction)),
      hooks,
    );
  (results, () => RemoteAction.send(~action=(), loadNextPageAction), hooks);
};

let getStories = hn =>
  switch (hn) {
  | None => []
  | Some(l) =>
    l.topStories
    |> Array.fold_left(
         acc =>
           fun
           | `Story(story) => [story, ...acc]
           | _ => acc,
         [],
       )
    |> List.rev
  };

let fetchStories = (~offset, ~pageSize) => {
  let query = Query.make(~offset, ~limit=pageSize, ());
  let parser = result => {
    getStories(query#parse(result)#hn)
    |> List.map(story =>
         {
           story,
           timeAgo:
             timeAgoSince(
               Core_kernel.Time.(
                 of_span_since_epoch(Span.of_int_sec(story.time))
               ),
             ),
         }
       );
  };

  GraphQLClient.get(query#query, ~variables=query#variables, parser);
};

let component = {
  open Brisk_macos;
  open Core_kernel.Sequence;
  let component = Brisk.component("TopStories");
  component(hooks => {
    let (stories, loadNextPage, hooks) = paging(fetchStories, 30, hooks);
    (
      hooks,
      <scrollView
        onReachedEnd=loadNextPage
        style=Brisk.Layout.[
          flex(1.),
          background(Color.hex("#fff")),
        ]>
        ...{
             stories
             |> getResultList
             |> List.rev
             |> of_list
             |> map(~f=of_list)
             |> concat
             |> mapi(~f=(index, astory) =>
                  <story story=astory index={index + 1} />
                )
             |> to_list
           }
      </scrollView>,
    );
  });
};
