open Brisk_macos;

type stories =
  array([ | `Poll(Story.poll) | `Story(Story.story) | `Job(Story.job)]);

module ListStyle = {
  open Brisk.Layout;
  let backgroundColor = background(Color.hex("#FBFBFB"));
  let highlightedBackgroundColor = background(Color.hex("#F2F2F2"));
  let detailsFont = font(~size=12., ());
};

let story = {
  open Brisk;
  let component = component("story");
  (
    ~story as {Story.story, timeAgo},
    ~index,
    ~goToComments,
    ~children as _: list(unit),
    (),
  ) =>
    component(hooks => {
      open Layout;
      let (isHighlighted, setHighlighted, hooks) = Hooks.state(false, hooks);
      let (commentsHighlighted, setCommentsHighlighted, hooks) =
        Hooks.state(false, hooks);

      let (onHover, onClick) =
        switch (story.url) {
        | Some(url) => (
            Some(hovered => setHighlighted(hovered)),
            Some(() => Brisk_macos.Std.openUrl(url)),
          )
        | None => (None, None)
        };
      (
        hooks,
        <clickable
          ?onHover
          ?onClick
          style=[
            height(47.),
            isHighlighted
              ? ListStyle.highlightedBackgroundColor
              : ListStyle.backgroundColor,
            padding4(~top=6., ~bottom=4., ()),
          ]>
          <view style=[alignItems(`Center), flexDirection(`Row)]>
            <text
              style=[
                height(20.),
                width(30.),
                align(`Center),
                font(~size=13., ()),
                color(Color.hex("#B0B0B0")),
              ]
              value={string_of_int(index)}
            />
            <view style=[flex(1.)]>
              <view style=[flexDirection(`Row)]>
                <text
                  style=[
                    flex(1.),
                    font(~size=13., ()),
                    lineBreak(`TruncateTail),
                    color(Color.hex("#333333")),
                  ]
                  value={story.title}
                />
              </view>
              <view style=[padding4(~top=4., ()), flexDirection(`Row)]>
                <text
                  style=[color(Color.hex("#888888")), ListStyle.detailsFont]
                  value={Format.sprintf("%i points | ", story.score)}
                />
                <clickable
                  onHover={hovered => setCommentsHighlighted(hovered)}
                  onClick=goToComments
                  style=[
                    height(16.),
                    commentsHighlighted
                      ? background(Color.hex("#E5E5E5"))
                      : background(Color.hex("#F5F5F5")),
                    padding4(~left=4., ~right=4., ()),
                    justifyContent(`Center),
                    border(
                      ~width=1.,
                      ~radius=4.,
                      ~color=Color.hex("#D4D4D4"),
                      (),
                    ),
                  ]>
                  <text
                    style=[
                      color(Color.hex("#888888")),
                      ListStyle.detailsFont,
                    ]
                    value={Format.sprintf("%i comments", story.descendants)}
                  />
                </clickable>
                <text
                  style=[color(Color.hex("#888888")), ListStyle.detailsFont]
                  value={
                    switch (Story.formatUrl(story.url)) {
                    | None => Format.sprintf(" | by %s", story.by.id)
                    | Some(url) =>
                      Format.sprintf(
                        " | by %s | source: %s",
                        story.by.id,
                        url,
                      )
                    }
                  }
                />
              </view>
            </view>
            <text
              style=[
                ListStyle.detailsFont,
                color(Color.hex("#ACACAC")),
                padding4(~left=3., ~right=3., ()),
              ]
              value=timeAgo
            />
          </view>
        </clickable>,
      );
    });
};

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
               Story.timeAgoSinceNow(
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
  open Core_kernel.Sequence;
  open Brisk.Layout;
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
        switch (stories) {
        | Loading =>
          <view
            style=[
              ListStyle.backgroundColor,
              justifyContent(`Center),
              alignItems(`Center),
              flex(1.),
            ]>
            <activityIndicator />
          </view>
        | stories =>
          <scrollView
            onReachedEnd=loadNextPage
            style=[flex(1.), ListStyle.backgroundColor]>
            {stories
             |> Paging.getResultList
             |> List.rev
             |> of_list
             |> map(~f=of_list)
             |> concat
             |> mapi(~f=(index, astory) =>
                  <story
                    story=astory
                    index={index + 1}
                    goToComments={() => showDetails(astory)}
                  />
                )
             |> to_list
             |> Brisk.listToElement}
          </scrollView>
        },
      );
    });
};
