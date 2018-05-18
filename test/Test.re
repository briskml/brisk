open ReasonReact;

open TestComponents;

open TestRenderer;

open Assert;

let suite = [
  (
    "Box wrapper",
    `Quick,
    () => {
      let twoBoxes =
        <Div id=2>
          <Box id=4 state="ImABox" />
          <Box id=5 state="ImABox" />
        </Div>;
      let oneBox = <Div id=2> <Box id=3 /> </Div>;
      start(<Components.BoxWrapper />)
      |> expect([
           <BoxWrapper id=1>
             <Div id=2> <Box id=3 state="ImABox" /> </Div>
           </BoxWrapper>
         ])
      |> update(<Components.BoxWrapper twoBoxes=true />)
      |> expect((
           [<BoxWrapper id=1> twoBoxes </BoxWrapper>],
           Some({
             subtreeChange: `Nested,
             updateLog:
               ref([
                 UpdateInstance({
                   stateChanged: false,
                   subTreeChanged:
                     `ReplaceElements((
                       [<Box id=3 state="ImABox" />],
                       [
                         <Box id=4 state="ImABox" />,
                         <Box id=5 state="ImABox" />
                       ]
                     )),
                   newInstance: twoBoxes,
                   oldInstance: oneBox
                 }),
                 UpdateInstance({
                   stateChanged: false,
                   subTreeChanged: `Nested,
                   newInstance: <BoxWrapper id=1> twoBoxes </BoxWrapper>,
                   oldInstance: <BoxWrapper id=1> oneBox </BoxWrapper>
                 })
               ])
           })
         ))
      |> done_;
    }
  ),
  (
    "Change counter test",
    `Quick,
    () => {
      let (beforeUpdate4, _) as testContinuation =
        start(<Components.ChangeCounter label="defaultText" />)
        |> expect([<ChangeCounter id=1 label="defaultText" counter=10 />])
        |> update(<Components.ChangeCounter label="defaultText" />)
        |> expect((
             [<ChangeCounter id=1 label="defaultText" counter=10 />],
             None
           ))
        |> update(<Components.ChangeCounter label="updatedText" />)
        |> expect((
             [<ChangeCounter id=1 label="updatedText" counter=11 />],
             Some({
               subtreeChange: `Nested,
               updateLog:
                 ref([
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance:
                       <ChangeCounter id=1 label="defaultText" counter=10 />,
                     newInstance:
                       <ChangeCounter id=1 label="updatedText" counter=11 />
                   })
                 ])
             })
           ))
        |> flushPendingUpdates
        |> expect(
             Some((
               [<ChangeCounter id=1 label="updatedText" counter=2011 />],
               [
                 TestRenderer.UpdateInstance({
                   stateChanged: true,
                   subTreeChanged: `NoChange,
                   oldInstance:
                     <ChangeCounter id=1 label="updatedText" counter=11 />,
                   newInstance:
                     <ChangeCounter id=1 label="updatedText" counter=2011 />
                 })
               ]
             ))
           )
        |> flushPendingUpdates
        |> expect(None)
        |> flushPendingUpdates
        |> expect(None)
        |> update(
             <Components.ButtonWrapperWrapper wrappedText="updatedText" />
           )
        |> expect(
             ~label=
               "Updating components: ChangeCounter to ButtonWrapperWrapper",
             (
               [
                 <ButtonWrapperWrapper
                   id=2
                   nestedText="wrappedText:updatedText"
                 />
               ],
               Some({
                 subtreeChange: `Nested,
                 updateLog:
                   ref([
                     ChangeComponent({
                       oldSubtree: [],
                       newSubtree: [
                         <Div id=3>
                           <Text id=4 title="buttonWrapperWrapperState" />
                           <Text id=5 title="wrappedText:updatedText" />
                           <ButtonWrapper id=6 />
                         </Div>
                       ],
                       oldInstance:
                         <ChangeCounter
                           id=1
                           label="updatedText"
                           counter=2011
                         />,
                       newInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="wrappedText:updatedText"
                         />
                     })
                   ])
               })
             )
           );
      let (afterUpdate, _) =
        testContinuation
        |> update(
             <Components.ButtonWrapperWrapper
               wrappedText="updatedTextmodified"
             />
           )
        |> expect(
             ~label="Updating text in the button wrapper",
             (
               [
                 <ButtonWrapperWrapper
                   id=2
                   nestedText="wrappedText:updatedTextmodified"
                 />
               ],
               Some({
                 subtreeChange: `Nested,
                 updateLog:
                   ref([
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `ContentChanged(`NoChange),
                       oldInstance:
                         <Text id=5 title="wrappedText:updatedText" />,
                       newInstance:
                         <Text id=5 title="wrappedText:updatedTextmodified" />
                     }),
                     UpdateInstance({
                       stateChanged: false,
                       subTreeChanged: `Nested,
                       oldInstance:
                         <Div id=3>
                           <Text id=4 title="buttonWrapperWrapperState" />
                           <Text id=5 title="wrappedText:updatedText" />
                           <ButtonWrapper id=6 />
                         </Div>,
                       newInstance:
                         <Div id=3>
                           <Text id=4 title="buttonWrapperWrapperState" />
                           <Text
                             id=5
                             title="wrappedText:updatedTextmodified"
                           />
                           <ButtonWrapper id=6 />
                         </Div>
                     }),
                     UpdateInstance({
                       stateChanged: false,
                       subTreeChanged: `Nested,
                       oldInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="wrappedText:updatedText"
                         />,
                       newInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="wrappedText:updatedTextmodified"
                         />
                     })
                   ])
               })
             )
           );
      check(
        Alcotest.bool,
        "Memoized nested button wrapper",
        true,
        switch (beforeUpdate4, afterUpdate) {
        | (
            IFlat(
              Instance({
                instanceSubTree:
                  IFlat(
                    Instance({instanceSubTree: INested(_, [_, _, IFlat(x)])})
                  )
              })
            ),
            IFlat(
              Instance({
                instanceSubTree:
                  IFlat(
                    Instance({instanceSubTree: INested(_, [_, _, IFlat(y)])})
                  )
              })
            )
          ) =>
          x === y
        | _ => false
        }
      );
    }
  ),
  (
    "Test Lists With Dynamic Keys",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.BoxList useDynamicKeys=true rAction />)
      |> expect(~label="Initial BoxList", [<BoxList id=1 />])
      |> act(~action=Components.BoxList.Create("Hello"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [
               <BoxList id=1>
                 <BoxWithDynamicKeys id=2 state="Hello" />
               </BoxList>
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [],
                     [<BoxWithDynamicKeys id=2 state="Hello" />]
                   )),
                 oldInstance: <BoxList id=1 />,
                 newInstance:
                   <BoxList id=1>
                     <BoxWithDynamicKeys id=2 state="Hello" />
                   </BoxList>
               })
             ]
           ))
         )
      |> act(~action=Components.BoxList.Create("World"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [
               <BoxList id=1>
                 <BoxWithDynamicKeys id=3 state="World" />
                 <BoxWithDynamicKeys id=2 state="Hello" />
               </BoxList>
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [<BoxWithDynamicKeys id=2 state="Hello" />],
                     [
                       <BoxWithDynamicKeys id=3 state="World" />,
                       <BoxWithDynamicKeys id=2 state="Hello" />
                     ]
                   )),
                 oldInstance:
                   <BoxList id=1>
                     <BoxWithDynamicKeys id=2 state="Hello" />
                   </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <BoxWithDynamicKeys id=3 state="World" />
                     <BoxWithDynamicKeys id=2 state="Hello" />
                   </BoxList>
               })
             ]
           ))
         )
      |> act(~action=Components.BoxList.Reverse, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [
               <BoxList id=1>
                 <BoxWithDynamicKeys id=2 state="Hello" />
                 <BoxWithDynamicKeys id=3 state="World" />
               </BoxList>
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Reordered,
                 oldInstance:
                   <BoxList id=1>
                     <BoxWithDynamicKeys id=3 state="World" />
                     <BoxWithDynamicKeys id=2 state="Hello" />
                   </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <BoxWithDynamicKeys id=2 state="Hello" />
                     <BoxWithDynamicKeys id=3 state="World" />
                   </BoxList>
               })
             ]
           ))
         )
      |> done_;
    }
  ),
  (
    "Test Lists Without Dynamic Keys",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.BoxList rAction />)
      |> expect(~label="Initial BoxList", [<BoxList id=1 />])
      |> act(~action=Components.BoxList.Create("Hello"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [<BoxList id=1> <Box id=2 state="Hello" /> </BoxList>],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements(([], [<Box id=2 state="Hello" />])),
                 oldInstance: <BoxList id=1 />,
                 newInstance:
                   <BoxList id=1> <Box id=2 state="Hello" /> </BoxList>
               })
             ]
           ))
         )
      |> act(~action=Components.BoxList.Create("World"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [
               <BoxList id=1>
                 <Box id=3 state="World" />
                 <Box id=4 state="Hello" />
               </BoxList>
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [<Box id=2 state="Hello" />],
                     [<Box id=3 state="World" />, <Box id=4 state="Hello" />]
                   )),
                 oldInstance:
                   <BoxList id=1> <Box id=2 state="Hello" /> </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <Box id=3 state="World" />
                     <Box id=4 state="Hello" />
                   </BoxList>
               })
             ]
           ))
         )
      |> act(~action=Components.BoxList.Reverse, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Add Hello then Flush",
           Some((
             [
               <BoxList id=1>
                 <Box id=3 state="Hello" />
                 <Box id=4 state="World" />
               </BoxList>
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Box id=4 state="Hello" />,
                 newInstance: <Box id=4 state="World" />
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Box id=3 state="World" />,
                 newInstance: <Box id=3 state="Hello" />
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Nested,
                 oldInstance:
                   <BoxList id=1>
                     <Box id=3 state="World" />
                     <Box id=4 state="Hello" />
                   </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <Box id=3 state="Hello" />
                     <Box id=4 state="World" />
                   </BoxList>
               })
             ]
           ))
         )
      |> done_;
    }
  ),
  (
    "Deep Move Box With Dynamic Keys",
    `Quick,
    () => {
      let box_ = <Components.BoxWithDynamicKeys title="box to move" />;
      let (beforeUpdate, _) as testContinuation =
        start(box_)
        |> expect(
             ~label="Initial Box",
             [<BoxWithDynamicKeys id=1 state="box to move" />]
           );
      let (afterUpdate, _) =
        testContinuation
        |> update(
             Nested(
               "div",
               [Components.stringToElement("before"), Nested("div", [box_])]
             )
           )
        |> expect(
             ~label="After update",
             (
               [
                 <Text id=2 title="before" />,
                 <BoxWithDynamicKeys id=1 state="box to move" />
               ],
               Some({
                 subtreeChange:
                   `ReplaceElements((
                     [<BoxWithDynamicKeys id=1 state="box to move" />],
                     [
                       <Text id=2 title="before" />,
                       <BoxWithDynamicKeys id=1 state="box to move" />
                     ]
                   )),
                 updateLog: ref([])
               })
             )
           );
      check(
        Alcotest.bool,
        "Memoized nested box",
        true,
        switch (beforeUpdate, afterUpdate) {
        | (IFlat(x), INested(_, [_, INested(_, [IFlat(y)])])) => x === y
        | _ => false
        }
      );
    }
  ),
  (
    "Test With Static Keys",
    `Quick,
    () => {
      let key1 = Key.create();
      let key2 = Key.create();
      start(
        listToElement([
          <Components.Box key=key1 title="Box1unchanged" />,
          <Components.Box key=key2 title="Box2unchanged" />
        ])
      )
      |> expect(
           ~label="Initial Boxes",
           [
             <Box id=key1 state="Box1unchanged" />,
             <Box id=key2 state="Box2unchanged" />
           ]
         )
      |> update(
           listToElement([
             <Components.Box key=key2 title="Box2changed" />,
             <Components.Box key=key1 title="Box1changed" />
           ])
         )
      |> expect(
           ~label="Swap Boxes",
           (
             [
               <Box id=key2 state="Box2changed" />,
               <Box id=key1 state="Box1changed" />
             ],
             Some(
               TestRenderer.{
                 subtreeChange: `Reordered,
                 updateLog:
                   ref([
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `ContentChanged(`NoChange),
                       oldInstance: <Box id=1 state="Box1unchanged" />,
                       newInstance: <Box id=1 state="Box1changed" />
                     }),
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `ContentChanged(`NoChange),
                       oldInstance: <Box id=2 state="Box2unchanged" />,
                       newInstance: <Box id=2 state="Box2changed" />
                     })
                   ])
               }
             )
           )
         )
      |> done_;
    }
  ),
  (
    "Test Update on Alternate Clicks",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.UpdateAlternateClicks rAction />)
      |> expect(
           ~label="Initial",
           [<UpdateAlternateClicks state="0" text="0" />]
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="First click then flush",
           Some((
             [<UpdateAlternateClicks state="1" text="0" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `NoChange,
                 oldInstance: <UpdateAlternateClicks state="0" text="0" />,
                 newInstance: <UpdateAlternateClicks state="1" text="0" />
               })
             ]
           ))
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Second click then flush",
           Some((
             [<UpdateAlternateClicks state="2" text="2" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Text id=2 title="0" />,
                 newInstance: <Text id=2 title="2" />
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Nested,
                 oldInstance: <UpdateAlternateClicks state="1" text="0" />,
                 newInstance: <UpdateAlternateClicks state="2" text="2" />
               })
             ]
           ))
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Third click then flush",
           Some((
             [<UpdateAlternateClicks state="3" text="2" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `NoChange,
                 oldInstance: <UpdateAlternateClicks state="2" text="2" />,
                 newInstance: <UpdateAlternateClicks state="3" text="2" />
               })
             ]
           ))
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="Fourth click then flush",
           Some((
             [<UpdateAlternateClicks state="4" text="4" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Text id=2 title="2" />,
                 newInstance: <Text id=2 title="4" />
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Nested,
                 oldInstance: <UpdateAlternateClicks state="3" text="2" />,
                 newInstance: <UpdateAlternateClicks state="4" text="4" />
               })
             ]
           ))
         )
      |> done_;
    }
  ),
  (
    "Test flat update",
    `Quick,
    () =>
      start(<Components.Text key=1 title="x" />)
      |> expect([<Text id=1 title="x" />])
      |> update(<Components.Text key=2 title="y" />)
      |> expect(
           ~label="Will return `ReplaceElements for top level flat update",
           (
             [<Text id=2 title="y" />],
             Some({
               subtreeChange:
                 `ReplaceElements((
                   [<Text id=1 title="x" />],
                   [<Text id=2 title="y" />]
                 )),
               updateLog: ref([])
             })
           )
         )
      |> done_
  ),
  (
    "Test no change",
    `Quick,
    () => {
      let key1 = Key.create();
      let key2 = Key.create();
      start(
        listToElement([
          <Components.Text key=key1 title="x" />,
          <Components.Text key=key2 title="y" />
        ])
      )
      |> expect([<Text id=1 title="x" />, <Text id=2 title="y" />])
      |> update(
           listToElement(
             Components.[
               <Text key=key1 title="x" />,
               <Text key=key2 title="y" />
             ]
           )
         )
      |> expect(
           ~label=
             "Updates the state because the state is updated with a new instance of string",
           (
             [<Text id=1 title="x" />, <Text id=2 title="y" />],
             Some({
               subtreeChange: `Nested,
               updateLog:
                 ref([
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance: <Text id=2 title="y" />,
                     newInstance: <Text id=2 title="y" />
                   }),
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance: <Text id=1 title="x" />,
                     newInstance: <Text id=1 title="x" />
                   })
                 ])
             })
           )
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key2 title="y" />,
               <Text key=key1 title="x" />
             ]
           )
         )
      |> expect(
           ~label=
             "Updates the state because the state is updated with a new instance",
           (
             [<Text id=2 title="y" />, <Text id=1 title="x" />],
             Some({
               subtreeChange: `Reordered,
               updateLog:
                 ref(
                   TestRenderer.[
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `NoChange,
                       oldInstance: <Text id=1 title="x" />,
                       newInstance: <Text id=1 title="x" />
                     }),
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `NoChange,
                       oldInstance: <Text id=2 title="y" />,
                       newInstance: <Text id=2 title="y" />
                     })
                   ]
                 )
             })
           )
         )
      |> done_;
    }
  ),
  (
    "Prepend Element",
    `Quick,
    () => {
      GlobalState.useTailHack := true;
      let key1 = Key.create();
      let key2 = Key.create();
      let commonElement = [<Components.Text key=key1 title="x" />];
      start(listToElement(commonElement))
      |> expect([Text.createElement(~id=1, ~title="x", ~children=(), ())])
      |> update(
           listToElement([
             <Components.Text key=key2 title="y" />,
             ...commonElement
           ])
         )
      |> expect((
           [<Text id=2 title="y" />, <Text id=1 title="x" />],
           Some({
             subtreeChange: `PrependElement([<Text id=2 title="y" />]),
             updateLog: ref([])
           })
         ))
      |> done_;
    }
  )
];

Alcotest.run(~argv=[|"--verbose --color"|], "Brisk", [("BoxWrapper", suite)]);
