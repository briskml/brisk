module StoryType = {
  type t =
    | Top
    | Show
    | New
    | Jobs;

  let toString =
    fun
    | Top => "Top"
    | Show => "Show"
    | New => "New"
    | Jobs => "Jobs";

  module Query = {
    type topStories = {topStories: StoryList.stories};
    type showStories = {showStories: StoryList.stories};
    type newStories = {newStories: StoryList.stories};
    type jobStories = {jobStories: StoryList.stories};

    type stories('a) = {hn: option('a)};

    open Story;

    module Fragments = [%graphql
      {|
  fragment story on Story @bsRecord {
       kids @bsRecord {
         text
         by @bsRecord {
	        id
         }
         time
       }
       title
       url
       score
       by @bsRecord {
	      id
       }
       time,
  descendants
     }
|}
    ];

    module Top = [%graphql
      {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   topStories(offset: $offset, limit: $limit) {
     ... on Story {
       ...Fragments.Story
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
    module Show = [%graphql
      {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   showStories(offset: $offset, limit: $limit) {
     ... on Story {
       ...Fragments.Story
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
    module Jobs = [%graphql
      {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   jobStories(offset: $offset, limit: $limit) {
     ... on Story {
       ...Fragments.Story
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
    module New = [%graphql
      {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   newStories(offset: $offset, limit: $limit) {
     ... on Story {
       ...Fragments.Story
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

    let toGraphQLClientQuery = (extractStories, query) => {
      {
        GraphQLClient.query: query#query,
        variables: query#variables,
        parse: json =>
          Core_kernel.Option.map(query#parse(json)#hn, ~f=extractStories),
      };
    };

    let make = (~storyType, ~offset, ~limit, ()) => {
      switch (storyType) {
      | Top =>
        Top.make(~offset, ~limit, ())
        |> toGraphQLClientQuery(x => x.topStories)
      | Jobs =>
        Jobs.make(~offset, ~limit, ())
        |> toGraphQLClientQuery(x => x.jobStories)
      | New =>
        New.make(~offset, ~limit, ())
        |> toGraphQLClientQuery(x => x.newStories)
      | Show =>
        Show.make(~offset, ~limit, ())
        |> toGraphQLClientQuery(x => x.showStories)
      };
    };
  };
};

let storiesScreen = {
  open Navigation;
  open Reconciler;
  let component = component("app");
  component(hooks => {
    open Brisk_macos;
    let (selectedStoryType, setSelectedStoryType, hooks) =
      Hooks.state(StoryType.Top, hooks);
    let (selectedStory, setSelectedStory, hooks) = Hooks.state(None, hooks);
    let storyItem = (~children as _: list(unit), ~storyType, ()) =>
      SegmentedControl.(
        <item
          isSelected={storyType === selectedStoryType}
          onClick={() => setSelectedStoryType(storyType)}>
          ...{StoryType.toString(storyType)}
        </item>
      );
    let segmentedControl = SegmentedControl.segmentedControl;
    Toolbar.(
      hooks,
      <screen
        toolbarItems=
          <>
            <flexibleSpace />
            <item>
              <segmentedControl>
                <storyItem storyType=Top />
                <storyItem storyType=New />
                <storyItem storyType=Show />
                <storyItem storyType=Jobs />
              </segmentedControl>
            </item>
            <flexibleSpace />
          </>
        contentView=StoryList.(
          <component
            showDetails={astory => setSelectedStory(Some(astory))}
            resource={StoryType.toString(selectedStoryType)}
            makeQuery={StoryType.Query.make(~storyType=selectedStoryType)}
          />
        )>
        {switch (selectedStory) {
         | None => empty
         | Some(story) =>
           StoryDetails.(
             <screen goBack={() => setSelectedStory(None)} story />
           )
         }}
      </screen>,
    );
  });
};
