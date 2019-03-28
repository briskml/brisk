let details =
    (
      ~children as _: list(unit),
      ~story as _: Story.storyWithRelativeTime,
      ~back as _: unit => unit,
      (),
    ) => Brisk_macos.Brisk.empty;

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
    module Top = [%graphql
      {|query stories($offset: Int!, $limit: Int!) {
  hn @bsRecord {
   topStories(offset: $offset, limit: $limit) {
     ... on Story @bsRecord {
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
     ... on Story @bsRecord {
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
     ... on Story @bsRecord {
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
     ... on Story @bsRecord {
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

let component = {
  open Brisk_macos;
  let component = Brisk.component("Stories");
  (~children as _: list(unit), ~renderToolbar, ()) =>
    component(hooks => {
      let (selectedStoryType, setSelectedStoryType, hooks) =
        Brisk.Hooks.state(StoryType.Top, hooks);
      let (selectedStory, setSelectedStory, hooks) =
        Brisk.Hooks.state(None, hooks);
      let hooks =
        Brisk.Hooks.effect(
          Always,
          () => {
            open StoryType;
            open Toolbar;
            let storyItem = (~children as _: list(unit), ~storyType, ()) =>
              SegmentedControl.(
                <item
                  isSelected={storyType === selectedStoryType}
                  onClick={() => setSelectedStoryType(storyType)}>
                  ...{toString(storyType)}
                </item>
              );
            let segmentedControl = SegmentedControl.segmentedControl;
            let () =
              if (selectedStory === None) {
                renderToolbar(
                  <toolbar>
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
                  </toolbar>,
                );
              } else {
                renderToolbar(
                  <toolbar>
                    <item>
                      <button
                        title="Open in Browser"
                        bezel=Cocoa.BriskButton.BezelStyle.TexturedRounded
                      />
                    </item>
                    <flexibleSpace />
                    <item>
                      <button
                        onClick={() => setSelectedStory(None)}
                        style=[Brisk.Layout.width(100.)]
                        image={`System(`LeftFacingTriangleTemplate)}
                        bezel=Cocoa.BriskButton.BezelStyle.TexturedRounded
                      />
                    </item>
                  </toolbar>,
                );
              };
            None;
          },
          hooks,
        );
      (
        hooks,
        switch (selectedStory) {
        | None =>
          StoryList.(
            <component
              showDetails={astory => setSelectedStory(Some(astory))}
              resource={StoryType.toString(selectedStoryType)}
              makeQuery={StoryType.Query.make(~storyType=selectedStoryType)}
            />
          )
        | Some(story) => StoryDetails.(<details story />)
        },
      );
    });
};
