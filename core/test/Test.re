open TestReactCore;
open Assert;

let root = Implementation.{name: "root", element: View};
let div = Implementation.{name: "Div", element: View};
let text = t => Implementation.{name: "Text", element: Text(t)};
let box = t => Implementation.{name: "Box", element: Text(t)};

let render = render(root);

let core = [
  (
    "Test rendered element mount",
    `Quick,
    () =>
      render(<Components.BoxWrapper />)
      |> executeSideEffects
      |> expect(
           ~label=
             "It correctly prepares a mount log, ignoring non-native BoxWrapper",
           [
             Implementation.BeginChanges,
             MountChild(div, box("ImABox"), 0),
             MountChild(root, div, 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test child elements list mount",
    `Quick,
    () =>
      render(
        Components.(
          <Div> <Box title="ImABox1" /> <Box title="ImABox2" /> </Div>
        ),
      )
      |> executeSideEffects
      |> expect(
           ~label="It mounts two boxes in a div",
           [
             Implementation.BeginChanges,
             MountChild(div, box("ImABox1"), 0),
             MountChild(div, box("ImABox2"), 1),
             MountChild(root, div, 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test element update top level mount",
    `Quick,
    () =>
      render(
        Components.(
          <Div> <Box title="ImABox1" /> <Box title="ImABox2" /> </Div>
        ),
      )
      |> executeSideEffects
      |> reset
      |> update(Components.(<Div> <Box title="ImABox3" /> </Div>))
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts topLevelUpdate",
           [
             Implementation.BeginChanges,
             UnmountChild(div, box("ImABox1")),
             UnmountChild(div, box("ImABox2")),
             MountChild(div, box("ImABox3"), 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test top level reorder",
    `Quick,
    () => {
      GlobalState.useTailHack := true;

      let key1 = Key.create();
      let key2 = Key.create();

      render(
        listToElement(
          Components.[
            <Text key=key1 title="x" />,
            <Text key=key2 title="y" />,
          ],
        ),
      )
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts top level list (for reorder)",
           [
             Implementation.BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             ChangeText("y", "y"),
             MountChild(root, text("y"), 1),
             CommitChanges,
           ],
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key2 title="y" />,
               <Text key=key1 title="x" />,
             ],
           ),
         )
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts reordered topLevelUpdate",
           [
             Implementation.BeginChanges,
             RemountChild(root, text("y"), 0),
             RemountChild(root, text("x"), 1),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
  (
    "Test top level replace elements",
    `Quick,
    () =>
      render(<Components.Text key=1 title="x" />)
      |> executeSideEffects
      |> expect(
           ~label=
             "It correctly mounts top level element (for replace elemets)",
           [
             Implementation.BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             CommitChanges,
           ],
         )
      |> update(<Components.Text key=2 title="y" />)
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts `ReplaceElements topLevelUpdate",
           [
             Implementation.BeginChanges,
             UnmountChild(root, text("x")),
             ChangeText("y", "y"),
             MountChild(root, text("y"), 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test subtree replace elements",
    `Quick,
    () => {
      let rAction = RemoteAction.create();

      let well = text("well");

      let testState =
        render(Components.(<ToggleClicks rAction />))
        |> executeSideEffects
        |> expect(
             ~label="It correctly mounts the tree",
             [
               Implementation.BeginChanges,
               ChangeText("well", "well"),
               MountChild(div, well, 0),
               MountChild(root, div, 0),
               CommitChanges,
             ],
           );

      RemoteAction.act(~action=Components.ToggleClicks.Click, rAction);
      let cell1 = text("cell1");
      let cell2 = text("cell2");

      testState
      |> flushPendingUpdates
      |> executeSideEffects
      |> expect(
           ~label="It correctly remounts subtree",
           [
             Implementation.BeginChanges,
             UnmountChild(div, well),
             ChangeText("cell1", "cell1"),
             MountChild(div, cell1, 0),
             ChangeText("cell2", "cell2"),
             MountChild(div, cell2, 0),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
  (
    "Test top level prepend",
    `Quick,
    () => {
      GlobalState.useTailHack := true;
      let key1 = Key.create();
      let key2 = Key.create();
      let commonElement = [<Components.Text key=key1 title="x" />];

      render(listToElement(commonElement))
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts top level list (for prepend)",
           [
             Implementation.BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             CommitChanges,
           ],
         )
      |> update(
           listToElement([
             <Components.Text key=key2 title="y" />,
             ...commonElement,
           ]),
         )
      |> executeSideEffects
      |> expect(
           ~label="It correctly mounts prepend topLevelUpdate",
           [
             Implementation.BeginChanges,
             ChangeText("y", "y"),
             MountChild(root, text("y"), 0),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
  (
    "Test simple subtree change",
    `Quick,
    () =>
      render(<Components.BoxWrapper />)
      |> executeSideEffects
      |> expect(
           ~label="It renders one Box inside a Div",
           [
             Implementation.BeginChanges,
             MountChild(div, box("ImABox"), 0),
             MountChild(root, div, 0),
             CommitChanges,
           ],
         )
      |> update(<Components.BoxWrapper twoBoxes=true />)
      |> executeSideEffects
      |> expect(
           ~label="It replaces one box with two boxes",
           [
             Implementation.BeginChanges,
             UnmountChild(div, box("ImABox")),
             MountChild(div, box("ImABox"), 0),
             MountChild(div, box("ImABox"), 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test willReceiveProps on ChangeCounter text update",
    `Quick,
    () =>
      /* TODO: This needs some love, we are testing state updates */
      render(<Components.ChangeCounter label="default text" />)
      |> expect(~label="It renders ChangeCounter component", [])
      |> update(<Components.ChangeCounter label="default text" />)
      |> expect(~label="It doesn't create any UpdateLog records", [])
      |> update(<Components.ChangeCounter label="updated text" />)
      |> expect(~label="It increments its local state on text change", [])
      |> flushPendingUpdates
      |> expect(
           ~label=
             "It flushes uncommited updates incrementing the counter by 10 on each update",
           [],
         )
      |> flushPendingUpdates
      |> expect(
           ~label="It flushes updates, but there are no pending actions",
           [],
         )
      |> flushPendingUpdates
      |> expect(
           ~label="It flushes updates, but there are no pending actions",
           [],
         )
      |> ignore,
  ),
  /*
     (
       "Test changing components",
       `Quick,
       () => {
         let {renderedElement: {renderedElement: beforeUpdate}} as testState =
           render(<Components.ChangeCounter label="default text" />)
           |> executeSideEffects
           |> expect(
                ~label="It renders ChangeCounter component",
                Implementation.[BeginChanges, CommitChanges],
              )
           |> update(
                <Components.ButtonWrapperWrapper wrappedText="initial text" />,
              )
           |> executeSideEffects
           |> expect(
                ~label=
                  "It changes components from ChangeCounter to ButtonWrapperWrapper",
                Implementation.[
                  BeginChanges,
                  ChangeText("initial text", "initial text"),
                  MountChild(div, text("initial text"), 0),
                  MountChild(div, div, 1),
                  MountChild(root, div, 0),
                  CommitChanges,
                ],
              );
         let {renderedElement: {renderedElement: afterUpdate}} =
           testState
           |> update(
                <Components.ButtonWrapperWrapper wrappedText="updated text" />,
              )
           |> executeSideEffects
           |> expect(
                ~label="It updates text in the ButtonWrapper",
                Implementation.[
                  BeginChanges,
                  ChangeText("initial text", "updated text"),
                  CommitChanges,
                ],
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
                       Instance({instanceSubTree: INested([_, IFlat(x)], _)}),
                     ),
                 }),
               ),
               IFlat(
                 Instance({
                   instanceSubTree:
                     IFlat(
                       Instance({instanceSubTree: INested([_, IFlat(y)], _)}),
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
   */
  /*
    (
      "Test BoxList with dynamic keys",
      `Quick,
      () => {
        let rAction = RemoteAction.create();
        render(<Components.BoxList useDynamicKeys=true rAction />)
        |> executeSideEffects
        |> expect(
             ~label="It renders initial BoxList",
             Implementation.[BeginChanges, CommitChanges],
           )
        |> act(~action=Components.BoxList.Create("Hello"), rAction)
        |> flushPendingUpdates
        |> executeSideEffects
        |> expect(
             ~label="It adds a new BoxItem and then flushes",
             Implementation.[
               BeginChanges,
               ChangeText("Hello", "Hello"),
               MountChild(root, text("Hello"), 0),
               CommitChanges,
             ],
           )
        |> act(~action=Components.BoxList.Create("World"), rAction)
        |> flushPendingUpdates
        |> executeSideEffects
        |> expect(
             ~label="It adds one more BoxItem and then flushes",
             Implementation.[
               BeginChanges,
               ChangeText("World", "World"),
               MountChild(root, text("World"), 0),
               CommitChanges,
             ],
           )
        |> act(~action=Components.BoxList.Reverse, rAction)
        |> flushPendingUpdates
        |> executeSideEffects
        |> expect(
             ~label="It reverses the items list in the BoxList",
             Implementation.[
               BeginChanges,
               RemountChild(root, text("Hello"), 0),
               CommitChanges,
             ],
           )
        |> ignore;
      },
    ),
   (
     "Test BoxList without dynamic keys",
     `Quick,
     () => {
       let rAction = RemoteAction.create();
       render(<Components.BoxList rAction />)
       |> executeSideEffects
       |> expect(
            ~label="It renders BoxList",
            Implementation.[BeginChanges, CommitChanges],
          )
       |> act(~action=Components.BoxList.Create("Hello"), rAction)
       |> flushPendingUpdates
       |> executeSideEffects
       |> expect(
            ~label="It adds a new Box and then flushes",
            Implementation.[
              BeginChanges,
              MountChild(root, box("Hello"), 0),
              CommitChanges,
            ],
          )
       |> act(~action=Components.BoxList.Create("World"), rAction)
       |> flushPendingUpdates
       |> executeSideEffects
       |> expect(
            ~label="It adds one more Box and then flushes",
            Implementation.[
              BeginChanges,
              MountChild(root, box("World"), 0),
              CommitChanges,
            ],
          )
       |> act(~action=Components.BoxList.Reverse, rAction)
       |> flushPendingUpdates
       |> executeSideEffects
       |> expect(
            ~label="It reverses the boxes list in the BoxList",
            Implementation.[
              BeginChanges,
              RemountChild(root, text("Hello"), 0),
              CommitChanges,
            ],
          )
       |> ignore;
     },
   ),
    */
  (
    "Test BoxItemDynamic memoizing during deep move",
    `Quick,
    () => {
      let box = <Components.BoxItemDynamic title="box to move" />;
      let {renderedElement: {renderedElement: beforeUpdate}} as testState =
        render(box)
        |> executeSideEffects
        |> expect(
             ~label="It renders the initial BoxItemDynamic",
             Implementation.[
               BeginChanges,
               ChangeText("box to move", "box to move"),
               MountChild(root, text("box to move"), 0),
               CommitChanges,
             ],
           );
      let {renderedElement: {renderedElement: afterUpdate}} =
        testState
        |> update(
             Nested([Components.stringToElement("before"), Nested([box])]),
           )
        |> executeSideEffects
        |> expect(
             ~label="It adds new element before BoxItemDynamic",
             Implementation.[
               BeginChanges,
               UnmountChild(root, text("box to move")),
               ChangeText("before", "before"),
               MountChild(root, text("before"), 0),
               MountChild(root, text("box to move"), 0),
               CommitChanges,
             ],
           );
      check(
        Alcotest.bool,
        "It memoized the nested BoxItemDynamic",
        true,
        switch (beforeUpdate, afterUpdate) {
        | (IFlat(x), INested([_, INested([IFlat(y)], _)], _)) => x === y
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
      render(
        listToElement([
          <Components.Box key=key1 title="Box1unchanged" />,
          <Components.Box key=key2 title="Box2unchanged" />,
        ]),
      )
      |> executeSideEffects
      |> expect(
           ~label="It renders the initial Boxes list",
           Implementation.[
             BeginChanges,
             MountChild(root, box("Box1unchanged"), 0),
             MountChild(root, box("Box2unchanged"), 1),
             CommitChanges,
           ],
         )
      |> update(
           listToElement([
             <Components.Box key=key2 title="Box2changed" />,
             <Components.Box key=key1 title="Box1changed" />,
           ]),
         )
      |> executeSideEffects
      |> expect(
           ~label="It reorders the list and updates each box",
           Implementation.[
             BeginChanges,
             RemountChild(root, box("Box2unchanged"), 0),
             RemountChild(root, box("Box1unchanged"), 1),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
  (
    "Test 'shouldUpdate' lifecycle phase",
    `Quick,
    () => {
      let rAction = RemoteAction.create();
      render(<Components.UpdateAlternateClicks rAction />)
      |> executeSideEffects
      |> expect(
           ~label="It renders UpdateAlternateClicks element",
           Implementation.[
             BeginChanges,
             ChangeText("0", "0"),
             MountChild(root, text("0"), 0),
             CommitChanges,
           ],
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> executeSideEffects
      |> expect(
           ~label="It only changes state on first click",
           Implementation.[BeginChanges, CommitChanges],
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> executeSideEffects
      |> expect(
           ~label="It changes both state and contents on second click",
           Implementation.[
             BeginChanges,
             ChangeText("0", "2"),
             CommitChanges,
           ],
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> executeSideEffects
      |> expect(
           ~label="It only changes state on third click",
           Implementation.[BeginChanges, CommitChanges],
         )
      |> act(~action=Components.UpdateAlternateClicks.Click, rAction)
      |> flushPendingUpdates
      |> executeSideEffects
      |> expect(
           ~label="It changes both state and contents on fourth click",
           Implementation.[
             BeginChanges,
             ChangeText("2", "4"),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
  (
    "Test top level flat update",
    `Quick,
    () =>
      render(<Components.Text key=1 title="x" />)
      |> executeSideEffects
      |> expect(
           ~label="It renders Text element",
           Implementation.[
             BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             CommitChanges,
           ],
         )
      |> update(<Components.Text key=2 title="y" />)
      |> executeSideEffects
      |> expect(
           ~label="It returns `ReplaceElements (not `Reordered!)",
           Implementation.[
             BeginChanges,
             UnmountChild(root, text("x")),
             ChangeText("y", "y"),
             MountChild(root, text("y"), 0),
             CommitChanges,
           ],
         )
      |> ignore,
  ),
  (
    "Test no change",
    `Quick,
    () => {
      let key1 = Key.create();
      let key2 = Key.create();
      render(
        listToElement([
          <Components.Text key=key1 title="x" />,
          <Components.Text key=key2 title="y" />,
        ]),
      )
      |> executeSideEffects
      |> expect(
           ~label="It renders list with Text elements",
           Implementation.[
             BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             ChangeText("y", "y"),
             MountChild(root, text("y"), 1),
             CommitChanges,
           ],
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key1 title="x" />,
               <Text key=key2 title="y" />,
             ],
           ),
         )
      |> executeSideEffects
      |> expect(
           ~label="It updates the state with a new instance of (same) string",
           Implementation.[BeginChanges, CommitChanges],
         )
      |> update(
           listToElement(
             Components.[
               <Text key=key2 title="y" />,
               <Text key=key1 title="x" />,
             ],
           ),
         )
      |> executeSideEffects
      |> expect(
           ~label=
             "It updates the state with a new instance and reorders the list",
           Implementation.[
             BeginChanges,
             RemountChild(root, text("y"), 0),
             RemountChild(root, text("x"), 1),
             CommitChanges,
           ],
         )
      |> ignore;
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
      render(listToElement(commonElement))
      |> executeSideEffects
      |> expect(
           ~label="It renders a new Text element",
           Implementation.[
             BeginChanges,
             ChangeText("x", "x"),
             MountChild(root, text("x"), 0),
             CommitChanges,
           ],
         )
      |> update(
           listToElement([
             <Components.Text key=key2 title="y" />,
             ...commonElement,
           ]),
         )
      |> executeSideEffects
      |> expect(
           ~label="It prepends a new Text element to the list",
           Implementation.[
             BeginChanges,
             ChangeText("y", "y"),
             MountChild(root, text("y"), 0),
             CommitChanges,
           ],
         )
      |> ignore;
    },
  ),
];

/** Annoying dune progress */
print_endline("");

Alcotest.run(~argv=[|"--verbose --color"|], "Brisk", [("Core", core)]);
