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

module Query = [%graphql
  {|{
  hn {
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
let topStories = GraphQLClient.get(query#query, query#parse);

let story = {
  open Brisk_macos;
  let component = Brisk.component("Story");
  (~story: story, ~index, ~children as _: list(unit), ()) =>
    component(hooks =>
      (
        hooks,
        <view>
          <text value={string_of_int(index)} />
          <text value={story.title} />
          {switch (story.url) {
           | Some(url) => <text value=url />
           | None => Brisk.empty
           }}
          <text value={string_of_int(story.score)} />
          <text value={story.by.id} />
          <text value={string_of_int(story.time)} />
          <text value={string_of_int(story.descendants)} />
        </view>,
      )
    );
};

let component = {
  open Brisk_macos;
  let component = Brisk.component("TopStories");
  component(hooks => (hooks, <view />));
};
