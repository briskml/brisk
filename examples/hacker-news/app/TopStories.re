type by = {id: string};

type story = {
  title: string,
  url: option(string),
  score: int,
  by,
  time: int,
  descendants: int,
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

let formatDetails = (~username, ~score, ~commentCount) => {
  Format.sprintf(
    "%i points | %i comments | by %s",
    score,
    commentCount,
    username,
  );
};

let formatUrl = {
  let regex = Re.seq([Re.start, Re.str("www.")]) |> Re.compile;
  let wrapInParens = str => Format.sprintf("(%s)", str);
  fun
  | Some(url) => {
      switch (Uri.host(Uri.of_string(url))) {
      | Some(url) when Re.execp(regex, url) =>
        Some(wrapInParens(String.sub(url, 4, String.length(url) - 4)))
      | Some(url) => Some(wrapInParens(url))
      | None => None
      };
    }
  | None => None;
};

let story = (~story: story, ~index, ~children as _: list(unit), ()) =>
  Brisk_macos.(
    Brisk.Layout.(
      <view style=[height(47.), background(Color.hex("#FFFFFF"))]>
        <view
          style=[
            alignItems(`Center),
            flexDirection(`Row),
            position(~top=0., ~bottom=0., ~left=0., ~right=0., `Absolute),
          ]>
          <text
            style=[
              font(~size=13., ()),
              color(Color.hex("#B0B0B0")),
              width(20.),
              /* Fixme, cannot fix any bc there'll be an exception */
              padding4(~left=5., ~right=5., ~top=0., ~bottom=0., ()),
            ]
            value={string_of_int(index)}
          />
          <view style=[Brisk.Layout.flex(1.)]>
            <view style=Brisk.Layout.[flexDirection(`Row)]>
              <text
                style=[font(~size=13., ()), color(Color.hex("#000000"))]
                value={story.title}
              />
              {switch (formatUrl(story.url)) {
               | Some(url) =>
                 <text
                   style=Brisk.Layout.[color(Color.hex("#ACACAC"))]
                   value=url
                 />
               | None => Brisk.empty
               }}
            </view>
            <text
              style=Brisk.Layout.[color(Color.hex("#888888"))]
              value={formatDetails(
                ~username=story.by.id,
                ~score=story.score,
                ~commentCount=story.descendants,
              )}
            />
          </view>
          <text value={string_of_int(story.time)} />
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
  let loadNext = () =>
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
    getStories(query#parse(result)#hn);
  };

  GraphQLClient.get(query#query, ~variables=query#variables, parser);
};

let component = {
  open Brisk_macos;
  open Core_kernel.Sequence;
  let component = Brisk.component("TopStories");
  component(hooks => {
    let (stories, _loadNextPage, hooks) = paging(fetchStories, 30, hooks);
    (
      hooks,
      <view>
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
      </view>,
    );
  });
};
