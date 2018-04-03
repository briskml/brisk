open Assert;

let suite =
  Components.[
    (
      "First Render",
      `Quick,
      () => {
        open TestRenderer;
        ReasonReact.GlobalState.reset();
        let component = BoxWrapper.make();
        let rendered = render(ReasonReact.element(component));
        let expected =
          TestComponents.[
            <BoxWrapper id=1>
              <Div id=2> <Box id=3 state="ImABox" /> </Div>
            </BoxWrapper>
          ];
        assertElement(expected, rendered);
      }
    ),
    (
      "Top level update",
      `Quick,
      () => {
        open TestRenderer;
        ReasonReact.GlobalState.reset();
        let actual =
          ReasonReact.RenderedElement.update(
            ReasonReact.RenderedElement.render(<BoxWrapper />),
            <BoxWrapper twoBoxes=true />
          );
        let twoBoxes =
          TestComponents.(
            <Div id=2>
              <Box id=4 state="ImABox" />
              <Box id=5 state="ImABox" />
            </Div>
          );
        let oneBox = TestComponents.(<Div id=2> <Box id=3 /> </Div>);
        let twoBoxesWrapper =
          TestComponents.(<BoxWrapper id=1> twoBoxes </BoxWrapper>);
        let expected = (
          [twoBoxesWrapper],
          [
            UpdateInstance({
              stateChanged: false,
              componentChanged: false,
              subTreeChanged:
                ReplaceElements(
                  [TestComponents.(<Box id=3 state="ImABox" />)],
                  TestComponents.[
                    <Box id=4 state="ImABox" />,
                    <Box id=5 state="ImABox" />
                  ]
                ),
              newInstance: twoBoxes,
              oldInstance: oneBox
            }),
            UpdateInstance({
              stateChanged: false,
              componentChanged: false,
              subTreeChanged: Nested,
              newInstance: twoBoxesWrapper,
              oldInstance:
                TestComponents.(<BoxWrapper id=1> oneBox </BoxWrapper>)
            }),
            TopLevelUpdate(Nested)
          ]
        );
        assertUpdate(expected, actual);
      }
    ),
    (
      "Change counter test",
      `Quick,
      () => {
        ReasonReact.GlobalState.reset();
        let rendered0 =
          ReasonReact.RenderedElement.render(
            <ChangeCounter label="defaultText" />
          );
        assertElement(
          [
            TestComponents.(
              <ChangeCounter id=1 label="defaultText" counter=10 />
            )
          ],
          rendered0
        );
        let (rendered1, _) as actual =
          TestRenderer.update(
            rendered0,
            <ChangeCounter label="defaultText" />
          );
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="defaultText" counter=10 />
              )
            ],
            []
          ),
          actual
        );
        let (rendered2, _) as actual =
          TestRenderer.update(
            rendered1,
            <ChangeCounter label="updatedText" />
          );
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="updatedText" counter=11 />
              )
            ],
            TestComponents.[
              UpdateInstance({
                componentChanged: false,
                stateChanged: true,
                subTreeChanged: NoChange,
                oldInstance:
                  <ChangeCounter id=1 label="defaultText" counter=10 />,
                newInstance:
                  <ChangeCounter id=1 label="updatedText" counter=11 />
              }),
              TopLevelUpdate(Nested)
            ]
          ),
          actual
        );
        let (rendered2f, _) as actual =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2);
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="updatedText" counter=2011 />
              )
            ],
            TestComponents.[
              UpdateInstance({
                componentChanged: false,
                stateChanged: true,
                subTreeChanged: NoChange,
                oldInstance:
                  <ChangeCounter id=1 label="updatedText" counter=11 />,
                newInstance:
                  <ChangeCounter id=1 label="updatedText" counter=2011 />
              })
            ]
          ),
          actual
        );
        let (rendered2f_mem, _) =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2f);
        check(
          Alcotest.bool,
          "it is memoized",
          rendered2f_mem === rendered2f,
          true
        );
        let (rendered2f_mem, _) =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2f_mem);
        check(
          Alcotest.bool,
          "it is memoized",
          rendered2f_mem === rendered2f,
          true
        );
        let (rendered3, _) =
          TestRenderer.update(
            rendered2f_mem,
            <ButtonWrapperWrapper wrappedText="updatedText" />
          );
        TestRenderer.convertElement(rendered3)
        |> check(
             renderedElement,
             "Switching Component Types from: ChangeCounter to ButtonWrapperWrapper",
             TestComponents.[
               <ButtonWrapperWrapper
                 id=2
                 nestedText="wrappedText:updatedText"
               />
             ]
           );
        let (rendered4, _) =
          TestRenderer.update(
            rendered3,
            <ButtonWrapperWrapper wrappedText="updatedTextmodified" />
          );
        TestRenderer.convertElement(rendered4)
        |> check(
             renderedElement,
             "Updating text in the button wrapper",
             TestComponents.[
               <ButtonWrapperWrapper
                 id=2
                 nestedText="wrappedText:updatedTextmodified"
               />
             ]
           );
        check(
          Alcotest.bool,
          "Memoized nested button wrapper",
          true,
          ReasonReact.(
            switch (rendered3, rendered4) {
            | (
                IFlat([
                  Instance({
                    instanceSubTree:
                      IFlat([
                        NativeInstance(
                          _,
                          {instanceSubTree: INested(_, [_, _, IFlat([x])])}
                        )
                      ])
                  })
                ]),
                IFlat([
                  Instance({
                    instanceSubTree:
                      IFlat([
                        NativeInstance(
                          _,
                          {instanceSubTree: INested(_, [_, _, IFlat([y])])}
                        )
                      ])
                  })
                ])
              ) =>
              x === y
            | _ => false
            }
          )
        );
      }
    ),
    (
      "Test Lists With Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let rAction = RemoteAction.create();
        let rendered0 =
          RenderedElement.render(<BoxList useDynamicKeys=true rAction />);
        RemoteAction.act(rAction, ~action=BoxList.Create("Hello"));
        let (rendered1, _) = RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) = RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) = RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial BoxList",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered2)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "World",
                     subtree: []
                   },
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered3)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "Hello",
                     subtree: []
                   },
                   {
                     id: 3,
                     component: Component(BoxWithDynamicKeys.component),
                     state: "World",
                     subtree: []
                   }
                 ]
               }
             ]
           );
      }
    ),
    (
      "Test Lists Without Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let rAction = RemoteAction.create();
        let rendered0 = RenderedElement.render(<BoxList rAction />);
        RemoteAction.act(rAction, ~action=BoxList.Create("Hello"));
        let (rendered1, _) = RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) = RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) = RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial BoxList",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 2,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered2)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(Box.component),
                     state: "World",
                     subtree: []
                   },
                   {
                     id: 4,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   }
                 ]
               }
             ]
           );
        TestRenderer.convertElement(rendered3)
        |> check(
             renderedElement,
             "Add Hello then Flush",
             [
               {
                 id: 1,
                 component: Component(BoxList.component),
                 state: "",
                 subtree: [
                   {
                     id: 3,
                     component: Component(Box.component),
                     state: "Hello",
                     subtree: []
                   },
                   {
                     id: 4,
                     component: Component(Box.component),
                     state: "World",
                     subtree: []
                   }
                 ]
               }
             ]
           );
      }
    ),
    (
      "Deep Move Box With Dynamic Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let box_ = <BoxWithDynamicKeys title="box to move" />;
        let rendered0 = RenderedElement.render(box_);
        let (rendered1, _) =
          RenderedElement.update(
            rendered0,
            Nested(
              "div",
              [ReasonReact.stringToElement("before"), Nested("div", [box_])]
            )
          );
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial Box",
             [
               {
                 id: 1,
                 component: Component(BoxWithDynamicKeys.component),
                 state: "box to move",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> check(
             renderedElement,
             "After update",
             [
               {
                 id: 2,
                 component: Component(Text.component),
                 state: "before",
                 subtree: []
               },
               {
                 id: 1,
                 component: Component(BoxWithDynamicKeys.component),
                 state: "box to move",
                 subtree: []
               }
             ]
           );
      }
    ),
    (
      "Test With Static Keys",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let key1 = Key.create();
        let key2 = Key.create();
        let rendered0 =
          RenderedElement.render(
            ReasonReact.listToElement([
              <Box key=key1 title="Box1unchanged" />,
              <Box key=key2 title="Box2unchanged" />
            ])
          );
        let (rendered1, _) =
          RenderedElement.update(
            rendered0,
            ReasonReact.listToElement([
              <Box key=key2 title="Box2changed" />,
              <Box key=key1 title="Box1changed" />
            ])
          );
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial Boxes",
             [
               {
                 id: 1,
                 component: Component(Box.component),
                 state: "Box1unchanged",
                 subtree: []
               },
               {
                 id: 2,
                 component: Component(Box.component),
                 state: "Box2unchanged",
                 subtree: []
               }
             ]
           );
        TestRenderer.convertElement(rendered1)
        |> check(
             renderedElement,
             "Swap Boxes",
             [
               {
                 id: 2,
                 component: Component(Box.component),
                 state: "Box2changed",
                 subtree: []
               },
               {
                 id: 1,
                 component: Component(Box.component),
                 state: "Box1changed",
                 subtree: []
               }
             ]
           );
      }
    ),
    (
      "Test Update on Alternate Clicks",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let result = (~state, ~text) => [
          {
            TestRenderer.id: 1,
            component: Component(UpdateAlternateClicks.component),
            state,
            subtree: [
              {
                id: 2,
                state: text,
                component: Component(Text.component),
                subtree: []
              }
            ]
          }
        ];
        let rAction = RemoteAction.create();
        let rendered =
          RenderedElement.render(<UpdateAlternateClicks rAction />);
        TestRenderer.convertElement(rendered)
        |> check(renderedElement, "Initial", result(~state="0", ~text="0"));
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        check(
          renderedElement,
          "First click then flush",
          result(~state="1", ~text="0"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        check(
          renderedElement,
          "Second click then flush",
          result(~state="2", ~text="2"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        check(
          renderedElement,
          "Second click then flush",
          result(~state="3", ~text="2"),
          TestRenderer.convertElement(rendered)
        );
        RemoteAction.act(rAction, ~action=Click);
        let (rendered, _) = RenderedElement.flushPendingUpdates(rendered);
        check(
          renderedElement,
          "Second click then flush",
          result(~state="4", ~text="4"),
          TestRenderer.convertElement(rendered)
        );
      }
    )
  ];

Alcotest.run(~argv=[|"--verbose --color"|], "Tests", [("BoxWrapper", suite)]);
