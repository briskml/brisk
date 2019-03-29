type stories =
  array([ | `Poll(Story.poll) | `Story(Story.story) | `Job(Story.job)]);

let story =
    (
      ~story as {Story.story, timeAgo},
      ~index,
      ~onClick,
      ~children as _: list(unit),
      (),
    ) =>
  Brisk_macos.(
    Brisk.Layout.(
      <clickable
        onClick style=[height(47.), background(Color.hex("#FFFFFF"))]>
        <view style=[alignItems(`Center), flexDirection(`Row)]>
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
                  color(Color.hex("#000000")),
                ]
                value={story.title}
              />
            </view>
            <text
              style=Brisk.Layout.[color(Color.hex("#888888"))]
              value={Story.formatDetails(
                ~username=story.by.id,
                ~score=story.score,
                ~commentCount=story.descendants,
                ~url=story.url,
              )}
            />
          </view>
          <text value=timeAgo />
        </view>
      </clickable>
    )
  );

let getStories = hn =>
  switch (hn) {
  | None => []
  | Some(l) =>
    l
    |> Array.fold_left(
         acc =>
           fun
           | `Story(story) => [story, ...acc]
           | _ => acc,
         [],
       )
    |> List.rev
  };

let fetchStories =
    (
      ~makeQuery:
         (~offset: int, ~limit: int, unit) =>
         GraphQLClient.query(option(stories)),
      ~offset,
      ~pageSize,
    ) => {
  let query = makeQuery(~offset, ~limit=pageSize, ());
  GraphQLClient.get({
    ...query,
    parse: json => {
      query.parse(json)
      |> getStories
      |> List.map(story =>
           {
             Story.story,
             timeAgo:
               Story.timeAgoSince(
                 Core_kernel.Time.(
                   of_span_since_epoch(Span.of_int_sec(story.time))
                 ),
               ),
           }
         );
    },
  });
};

let component = {
  open Brisk_macos;
  open Core_kernel.Sequence;
  let component = Brisk.component("StoryList");
  (
    ~children as _: list(unit),
    ~showDetails,
    ~resource: string,
    ~makeQuery,
    (),
  ) =>
    component(hooks => {
      let (stories, loadNextPage, hooks) =
        Paging.hook(fetchStories(~makeQuery), resource, 30, hooks);
      (
        hooks,
        <scrollView
          onReachedEnd=loadNextPage
          style=Brisk.Layout.[flex(1.), background(Color.hex("#fff"))]>
          {
               switch (stories) {
               | Loading =>
                 <activityIndicator style=Brisk.Layout.[flex(1.)] />
               | stories =>
                 stories
                 |> Paging.getResultList
                 |> List.rev
                 |> of_list
                 |> map(~f=of_list)
                 |> concat
                 |> mapi(~f=(index, astory) =>
                      <story
                        story=astory
                        index={index + 1}
                        onClick={() => showDetails(astory)}
                      />
                    )
                 |> to_list
                 |> Brisk.listToElement
               }
             }
        </scrollView>,
      );
    });
};