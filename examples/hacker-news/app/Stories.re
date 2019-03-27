let details =
    (
      ~children as _: list(unit),
      ~story as _: TopStories.storyWithRelativeTime,
      ~back as _: unit => unit,
      (),
    ) => Brisk_macos.Brisk.empty;

type storyType =
  | Top;

let component = {
  open Brisk_macos;
  let component = Brisk.component("Stories");
  component(hooks => {
    let (_storyType, _, hooks) = Brisk.Hooks.state(Top, hooks);
    let (selectedStory, setSelectedStory, hooks) =
      Brisk.Hooks.state(None, hooks);
    (
      hooks,
      switch (selectedStory) {
      | Some(story) => <details story back={() => setSelectedStory(None)} />
      | None =>
        TopStories.(
          <component showDetails={story => setSelectedStory(Some(story))} />
        )
      },
    );
  });
};
