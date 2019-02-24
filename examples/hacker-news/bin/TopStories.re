type by = {id: string};

type story = {
  title: string,
  url: option(string),
  score: int,
  by,
  time: int,
};

type poll = {
  title: string,
};

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
       time
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

let component = {
  open Brisk_macos;
  let component = Brisk.component("TopStories");
  component(hooks => (hooks, <view />));
};
