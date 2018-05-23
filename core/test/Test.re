open TestReactCore;
open TestComponents;
open TestRenderer;

open Assert;

let suites = [
  (
    "Test simple subtree change",
    `Quick,
    () => {
      let twoBoxes =
        <Div id=2>
          <Box id=4 state="ImABox" />
          <Box id=5 state="ImABox" />
        </Div>;
      let oneBox = <Div id=2> <Box id=3 /> </Div>;
      start(<Components.BoxWrapper />)
      |> expect(
           ~label="It renders one Box inside a Div",
           [
             <BoxWrapper id=1>
               <Div id=2> <Box id=3 state="ImABox" /> </Div>
             </BoxWrapper>,
           ],
         )
      |> update(<Components.BoxWrapper twoBoxes=true />)
      |> expect(
           ~label="It replaces one box with two boxes",
           (
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
                           <Box id=5 state="ImABox" />,
                         ],
                       )),
                     newInstance: twoBoxes,
                     oldInstance: oneBox,
                   }),
                   UpdateInstance({
                     stateChanged: false,
                     subTreeChanged: `Nested,
                     newInstance: <BoxWrapper id=1> twoBoxes </BoxWrapper>,
                     oldInstance: <BoxWrapper id=1> oneBox </BoxWrapper>,
                   }),
                 ]),
             }),
           ),
         )
      |> done_;
    },
  ),
  (
    "Test willReceiveProps on ChangeCounter text update",
    `Quick,
    () =>
      start(<Components.ChangeCounter label="default text" />)
      |> expect(
           ~label="It renders ChangeCounter component",
           [<ChangeCounter id=1 label="default text" counter=0 />],
         )
      |> update(<Components.ChangeCounter label="default text" />)
      |> expect(
           ~label="It doesn't create any UpdateLog records",
           ([<ChangeCounter id=1 label="default text" counter=0 />], None),
         )
      |> update(<Components.ChangeCounter label="updated text" />)
      |> expect(
           ~label="It increments its local state on text change",
           (
             [<ChangeCounter id=1 label="updated text" counter=1 />],
             Some({
               subtreeChange: `Nested,
               updateLog:
                 ref([
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance:
                       <ChangeCounter id=1 label="default text" counter=0 />,
                     newInstance:
                       <ChangeCounter id=1 label="updated text" counter=1 />,
                   }),
                 ]),
             }),
           ),
         )
      |> flushPendingUpdates
      |> expect(
           ~label=
             "It flushes uncommited updates incrementing the counter by 10 on each update",
           Some((
             [<ChangeCounter id=1 label="updated text" counter=21 />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `NoChange,
                 oldInstance:
                   <ChangeCounter id=1 label="updated text" counter=1 />,
                 newInstance:
                   <ChangeCounter id=1 label="updated text" counter=21 />,
               }),
             ],
           )),
         )
      |> flushPendingUpdates
      |> expect(
           ~label="It flushes updates, but there are no pending actions",
           None,
         )
      |> flushPendingUpdates
      |> expect(
           ~label="It flushes updates, but there are no pending actions",
           None,
         )
      |> done_,
  ),
  (
    "Test changing components",
    `Quick,
    () => {
      let (beforeUpdate, _) as testContinuation =
        start(<Components.ChangeCounter label="default text" />)
        |> expect(
             ~label="It renders ChangeCounter component",
             [<ChangeCounter id=1 label="default text" counter=0 />],
           )
        |> update(
             <Components.ButtonWrapperWrapper wrappedText="initial text" />,
           )
        |> expect(
             ~label=
               "It changes components from ChangeCounter to ButtonWrapperWrapper",
             (
               [<ButtonWrapperWrapper id=2 nestedText="initial text" />],
               Some({
                 subtreeChange: `Nested,
                 updateLog:
                   ref([
                     ChangeComponent({
                       oldSubtree: [],
                       newSubtree: [
                         <Div id=3>
                           <Text id=4 title="initial text" />
                           <ButtonWrapper id=5 />
                         </Div>,
                       ],
                       oldInstance:
                         <ChangeCounter id=1 label="default text" counter=0 />,
                       newInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="initial text"
                         />,
                     }),
                   ]),
               }),
             ),
           );
      let (afterUpdate, _) =
        testContinuation
        |> update(
             <Components.ButtonWrapperWrapper wrappedText="updated text" />,
           )
        |> expect(
             ~label="It updates text in the ButtonWrapper",
             (
               [<ButtonWrapperWrapper id=2 nestedText="updated text" />],
               Some({
                 subtreeChange: `Nested,
                 updateLog:
                   ref([
                     UpdateInstance({
                       stateChanged: true,
                       subTreeChanged: `ContentChanged(`NoChange),
                       oldInstance: <Text id=4 title="initial text" />,
                       newInstance: <Text id=4 title="updated text" />,
                     }),
                     UpdateInstance({
                       stateChanged: false,
                       subTreeChanged: `Nested,
                       oldInstance:
                         <Div id=3>
                           <Text id=4 title="initial text" />
                           <ButtonWrapper id=5 />
                         </Div>,
                       newInstance:
                         <Div id=3>
                           <Text id=4 title="updated text" />
                           <ButtonWrapper id=5 />
                         </Div>,
                     }),
                     UpdateInstance({
                       stateChanged: false,
                       subTreeChanged: `Nested,
                       oldInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="initial text"
                         />,
                       newInstance:
                         <ButtonWrapperWrapper
                           id=2
                           nestedText="updated text"
                         />,
                     }),
                   ]),
               }),
             ),
           );
      check(
        Alcotest.bool,
        "It memoizes nested ButtonWrapper instance",
        true,
        switch (beforeUpdate, afterUpdate) {
        | (
            IFlat(
              Instance({
                instanceSubTree:
                  IFlat(
                    Instance({instanceSubTree: INested(_, [_, IFlat(x)])}),
                  ),
              }),
            ),
            IFlat(
              Instance({
                instanceSubTree:
                  IFlat(
                    Instance({instanceSubTree: INested(_, [_, IFlat(y)])}),
                  ),
              }),
            ),
          ) =>
          x === y
        | _ => false
        },
      );
    },
  ),
  (
    "Test BoxList with dynamic keys",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.BoxList useDynamicKeys=true rAction />)
      |> expect(~label="It renders initial BoxList", [<BoxList id=1 />])
      |> act(~action=Components.BoxList.Create("Hello"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It adds a new BoxItem and then flushes",
           Some((
             [
               <BoxList id=1> <BoxItemDynamic id=2 state="Hello" /> </BoxList>,
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [],
                     [<BoxItemDynamic id=2 state="Hello" />],
                   )),
                 oldInstance: <BoxList id=1 />,
                 newInstance:
                   <BoxList id=1>
                     <BoxItemDynamic id=2 state="Hello" />
                   </BoxList>,
               }),
             ],
           )),
         )
      |> act(~action=Components.BoxList.Create("World"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It adds one more BoxItem and then flushes",
           Some((
             [
               <BoxList id=1>
                 <BoxItemDynamic id=3 state="World" />
                 <BoxItemDynamic id=2 state="Hello" />
               </BoxList>,
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [<BoxItemDynamic id=2 state="Hello" />],
                     [
                       <BoxItemDynamic id=3 state="World" />,
                       <BoxItemDynamic id=2 state="Hello" />,
                     ],
                   )),
                 oldInstance:
                   <BoxList id=1>
                     <BoxItemDynamic id=2 state="Hello" />
                   </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <BoxItemDynamic id=3 state="World" />
                     <BoxItemDynamic id=2 state="Hello" />
                   </BoxList>,
               }),
             ],
           )),
         )
      |> act(~action=Components.BoxList.Reverse, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It reverses the items list in the BoxList",
           Some((
             [
               <BoxList id=1>
                 <BoxItemDynamic id=2 state="Hello" />
                 <BoxItemDynamic id=3 state="World" />
               </BoxList>,
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Reordered,
                 oldInstance:
                   <BoxList id=1>
                     <BoxItemDynamic id=3 state="World" />
                     <BoxItemDynamic id=2 state="Hello" />
                   </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <BoxItemDynamic id=2 state="Hello" />
                     <BoxItemDynamic id=3 state="World" />
                   </BoxList>,
               }),
             ],
           )),
         )
      |> done_;
    },
  ),
  (
    "Test BoxList without dynamic keys",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.BoxList rAction />)
      |> expect(~label="It renders BoxList", [<BoxList id=1 />])
      |> act(~action=Components.BoxList.Create("Hello"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It adds a new Box and then flushes",
           Some((
             [<BoxList id=1> <Box id=2 state="Hello" /> </BoxList>],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements(([], [<Box id=2 state="Hello" />])),
                 oldInstance: <BoxList id=1 />,
                 newInstance:
                   <BoxList id=1> <Box id=2 state="Hello" /> </BoxList>,
               }),
             ],
           )),
         )
      |> act(~action=Components.BoxList.Create("World"), rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It adds one more Box and then flushes",
           Some((
             [
               <BoxList id=1>
                 <Box id=3 state="World" />
                 <Box id=4 state="Hello" />
               </BoxList>,
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged:
                   `ReplaceElements((
                     [<Box id=2 state="Hello" />],
                     [<Box id=3 state="World" />, <Box id=4 state="Hello" />],
                   )),
                 oldInstance:
                   <BoxList id=1> <Box id=2 state="Hello" /> </BoxList>,
                 newInstance:
                   <BoxList id=1>
                     <Box id=3 state="World" />
                     <Box id=4 state="Hello" />
                   </BoxList>,
               }),
             ],
           )),
         )
      |> act(~action=Components.BoxList.Reverse, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It reverses the boxes list in the BoxList",
           Some((
             [
               <BoxList id=1>
                 <Box id=3 state="Hello" />
                 <Box id=4 state="World" />
               </BoxList>,
             ],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Box id=4 state="Hello" />,
                 newInstance: <Box id=4 state="World" />,
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Box id=3 state="World" />,
                 newInstance: <Box id=3 state="Hello" />,
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
                   </BoxList>,
               }),
             ],
           )),
         )
      |> done_;
    },
  ),
  (
    "Test BoxItemDynamic memoizing during deep move",
    `Quick,
    () => {
      let box_ = <Components.BoxItemDynamic title="box to move" />;
      let (beforeUpdate, _) as testContinuation =
        start(box_)
        |> expect(
             ~label="It renders the initial BoxItemDynamic",
             [<BoxItemDynamic id=1 state="box to move" />],
           );
      let (afterUpdate, _) =
        testContinuation
        |> update(
             Nested(
               "div",
               [
                 Components.stringToElement("before"),
                 Nested("div", [box_]),
               ],
             ),
           )
        |> expect(
             ~label="It adds new element before BoxItemDynamic",
             (
               [
                 <Text id=2 title="before" />,
                 <BoxItemDynamic id=1 state="box to move" />,
               ],
               Some({
                 subtreeChange:
                   `ReplaceElements((
                     [<BoxItemDynamic id=1 state="box to move" />],
                     [
                       <Text id=2 title="before" />,
                       <BoxItemDynamic id=1 state="box to move" />,
                     ],
                   )),
                 updateLog: ref([]),
               }),
             ),
           );
      check(
        Alcotest.bool,
        "It memoized the nested BoxItemDynamic",
        true,
        switch (beforeUpdate, afterUpdate) {
        | (IFlat(x), INested(_, [_, INested(_, [IFlat(y)])])) => x === y
        | _ => false
        },
      );
    },
  ),
  (
    "Test list updates with static keys",
    `Quick,
    () => {
      let key1 = Key.create();
      let key2 = Key.create();
      start(
        listToElement([
          <Components.Box key=key1 title="Box1unchanged" />,
          <Components.Box key=key2 title="Box2unchanged" />,
        ]),
      )
      |> expect(
           ~label="It renders the initial Boxes list",
           [
             <Box id=key1 state="Box1unchanged" />,
             <Box id=key2 state="Box2unchanged" />,
           ],
         )
      |> update(
           listToElement([
             <Components.Box key=key2 title="Box2changed" />,
             <Components.Box key=key1 title="Box1changed" />,
           ]),
         )
      |> expect(
           ~label="It reorders the list and updates each box",
           (
             [
               <Box id=key2 state="Box2changed" />,
               <Box id=key1 state="Box1changed" />,
             ],
             Some({
               subtreeChange: `Reordered,
               updateLog:
                 ref([
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `ContentChanged(`NoChange),
                     oldInstance: <Box id=1 state="Box1unchanged" />,
                     newInstance: <Box id=1 state="Box1changed" />,
                   }),
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `ContentChanged(`NoChange),
                     oldInstance: <Box id=2 state="Box2unchanged" />,
                     newInstance: <Box id=2 state="Box2changed" />,
                   }),
                 ]),
             }),
           ),
         )
      |> done_;
    },
  ),
  (
    "Test 'shouldUpdate' lifecycle phase",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      start(<Components.UpdateAlternateClicks rAction />)
      |> expect(
           ~label="It renders UpdateAlternateClicks element",
           [<UpdateAlternateClicks state="0" text="0" />],
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It only changes state on first click",
           Some((
             [<UpdateAlternateClicks state="1" text="0" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `NoChange,
                 oldInstance: <UpdateAlternateClicks state="0" text="0" />,
                 newInstance: <UpdateAlternateClicks state="1" text="0" />,
               }),
             ],
           )),
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It changes both state and contents on second click",
           Some((
             [<UpdateAlternateClicks state="2" text="2" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Text id=2 title="0" />,
                 newInstance: <Text id=2 title="2" />,
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Nested,
                 oldInstance: <UpdateAlternateClicks state="1" text="0" />,
                 newInstance: <UpdateAlternateClicks state="2" text="2" />,
               }),
             ],
           )),
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It only changes state on third click",
           Some((
             [<UpdateAlternateClicks state="3" text="2" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `NoChange,
                 oldInstance: <UpdateAlternateClicks state="2" text="2" />,
                 newInstance: <UpdateAlternateClicks state="3" text="2" />,
               }),
             ],
           )),
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> expect(
           ~label="It changes both state and contents on fourth click",
           Some((
             [<UpdateAlternateClicks state="4" text="4" />],
             [
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `ContentChanged(`NoChange),
                 oldInstance: <Text id=2 title="2" />,
                 newInstance: <Text id=2 title="4" />,
               }),
               UpdateInstance({
                 stateChanged: true,
                 subTreeChanged: `Nested,
                 oldInstance: <UpdateAlternateClicks state="3" text="2" />,
                 newInstance: <UpdateAlternateClicks state="4" text="4" />,
               }),
             ],
           )),
         )
      |> done_;
    },
  ),
  (
    "Test top level flat update",
    `Quick,
    () =>
      start(<Components.Text key=1 title="x" />)
      |> expect(~label="It renders Text element", [<Text id=1 title="x" />])
      |> update(<Components.Text key=2 title="y" />)
      |> expect(
           ~label="It returns `ReplaceElements (not `Reordered!)",
           (
             [<Text id=2 title="y" />],
             Some({
               subtreeChange:
                 `ReplaceElements((
                   [<Text id=1 title="x" />],
                   [<Text id=2 title="y" />],
                 )),
               updateLog: ref([]),
             }),
           ),
         )
      |> done_,
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
          <Components.Text key=key2 title="y" />,
        ]),
      )
      |> expect(
           ~label="It renders list with Text elements",
           [<Text id=1 title="x" />, <Text id=2 title="y" />],
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key1 title="x" />,
               <Text key=key2 title="y" />,
             ],
           ),
         )
      |> expect(
           ~label="It updates the state with a new instance of (same) string",
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
                     newInstance: <Text id=2 title="y" />,
                   }),
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance: <Text id=1 title="x" />,
                     newInstance: <Text id=1 title="x" />,
                   }),
                 ]),
             }),
           ),
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key2 title="y" />,
               <Text key=key1 title="x" />,
             ],
           ),
         )
      |> expect(
           ~label=
             "It updates the state with a new instance and reorders the list",
           (
             [<Text id=2 title="y" />, <Text id=1 title="x" />],
             Some({
               subtreeChange: `Reordered,
               updateLog:
                 ref([
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance: <Text id=1 title="x" />,
                     newInstance: <Text id=1 title="x" />,
                   }),
                   UpdateInstance({
                     stateChanged: true,
                     subTreeChanged: `NoChange,
                     oldInstance: <Text id=2 title="y" />,
                     newInstance: <Text id=2 title="y" />,
                   }),
                 ]),
             }),
           ),
         )
      |> done_;
    },
  ),
  (
    "Test prepending new element",
    `Quick,
    () => {
      GlobalState.useTailHack := true;
      let key1 = Key.create();
      let key2 = Key.create();
      let commonElement = [<Components.Text key=key1 title="x" />];
      start(listToElement(commonElement))
      |> expect(
           ~label="It renders a new Text element",
           [Text.createElement(~id=1, ~title="x", ~children=(), ())],
         )
      |> update(
           listToElement([
             <Components.Text key=key2 title="y" />,
             ...commonElement,
           ]),
         )
      |> expect(
           ~label="It prepends a new Text element to the list",
           (
             [<Text id=2 title="y" />, <Text id=1 title="x" />],
             Some({
               subtreeChange: `PrependElement([<Text id=2 title="y" />]),
               updateLog: ref([]),
             }),
           ),
         )
      |> done_;
    },
  ),
];

/** Annoying dune progress */
print_endline("");

Alcotest.run(~argv=[|"--verbose --color"|], "Brisk", [("Core", suites)]);
