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

type hn = {topStories: array([ | `Poll(poll) | `Story(story)])};

module Query = [%graphql
  {|{
  hn @bsRecord {
   topStories {
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
   }
  }
}|}
];

let query = Query.make();

let story = {
  open Brisk_macos;
  let component = Brisk.component("Story");
  (~story: story, ~index, ~children as _: list(unit), ()) =>
    component(hooks =>
      Brisk.Layout.(
        hooks,
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
                <text value={story.title} />
                {switch (story.url) {
                 | Some(url) => <text value=url />
                 | None => Brisk.empty
                 }}
              </view>
              <text
                value={
                  string_of_int(story.score)
                  ++ story.by.id
                  ++ string_of_int(story.descendants)
                }
              />
            </view>
            <text value={string_of_int(story.time)} />
          </view>
        </view>,
      )
    );
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

let component = {
  open Brisk_macos;
  let component = Brisk.component("TopStories");
  component(hooks => {
    let (stories, setStories, hooks) = Brisk.Hooks.state([], hooks);
    let hooks =
      Brisk.Hooks.effect(
        OnMount,
        () => {
          GraphQLClient.get(query#query, query#parse)
          |> Lwt.map(
               fun
               | Ok(topLevelItems) =>
                 setStories(getStories(topLevelItems#hn))
               | _ => (),
             )
          |> ignore;
          None;
        },
        hooks,
      );
    (
      hooks,
      <view>
        ...{stories |> List.map(astory => <story story=astory index=1 />)}
      </view>,
    );
  });
};
