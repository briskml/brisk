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
  (~children as _: list(unit), /* ~renderToolbar, */ ()) =>
    component(hooks => {
      let (_storyType, _, hooks) = Brisk.Hooks.state(Top, hooks);
      let (selectedStory, setSelectedStory, hooks) =
        Brisk.Hooks.state(None, hooks);
      /*
       let hooks =
         Brisk.Hooks.effect(
           If((!==), selectedStory),
           () => {
             open Toolbar;
             let () =
               if (selectedStory === None) {
                 renderToolbar(
                   <toolbar>
                     <flexibleSpace />
                     <item>
                       SegmentedControl.(
                         <segmentedControl>
                           <item onClick={() => print_endline("TOP")}>
                             ..."Top"
                           </item>
                           <item onClick={() => print_endline("New")}>
                             ..."New"
                           </item>
                           <item onClick={() => ()}> ..."Show" </item>
                           <item onClick={() => ()}> ..."Jobs" </item>
                         </segmentedControl>
                       )
                     </item>
                     <flexibleSpace />
                   </toolbar>,
                 );
               } else {
                 renderToolbar(
                   <toolbar>
                     <item> <button /> </item>
                     <flexibleSpace />
                     <item> <button /> </item>
                   </toolbar>,
                 );
               };
             None;
           },
           hooks,
         );
         */
      (
        hooks,
        switch (selectedStory) {
        | Some(story) =>
          <details story back={() => setSelectedStory(None)} />
        | None =>
          TopStories.(
            <component
              showDetails={story => setSelectedStory(Some(story))}
            />
          )
        },
      );
    });
};
