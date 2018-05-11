open Assert;

let suite =
  Components.[
    (
      "Box wrapper",
      `Quick,
      () => {
        open TestRenderer;
        open ReasonReact;
        GlobalState.reset();
        let previousReactElement = <BoxWrapper />;
        let rendered = render(previousReactElement);
        let expected =
          TestComponents.[
            <BoxWrapper id=1>
              <Div id=2> <Box id=3 state="ImABox" /> </Div>
            </BoxWrapper>
          ];
        assertElement(~label="First render", expected, rendered);
        let actual =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered,
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
          Some({
            subtreeChange: `Nested,
            updateLog:
              ref(
                TestComponents.[
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
                    newInstance: twoBoxesWrapper,
                    oldInstance: <BoxWrapper id=1> oneBox </BoxWrapper>
                  })
                ]
              )
          })
        );
        assertUpdate(expected, actual);
      }
    ),
    (
      "Change counter test",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let previousReactElement = <ChangeCounter label="defaultText" />;
        let rendered0 = RenderedElement.render(previousReactElement);
        assertElement(
          [
            TestComponents.(
              <ChangeCounter id=1 label="defaultText" counter=10 />
            )
          ],
          rendered0
        );
        let updatedReactElement = <ChangeCounter label="defaultText" />;
        let (rendered1, _) as actual =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered0,
            updatedReactElement
          );
        let previousReactElement = updatedReactElement;
        assertUpdate(
          (
            [
              TestComponents.(
                <ChangeCounter id=1 label="defaultText" counter=10 />
              )
            ],
            None
          ),
          actual
        );
        let updatedReactElement = <ChangeCounter label="updatedText" />;
        let (rendered2, _) as actual2 =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered1,
            updatedReactElement
          );
        let previousReactElement = updatedReactElement;
        assertUpdate(
          TestComponents.(
            [<ChangeCounter id=1 label="updatedText" counter=11 />],
            Some(
              TestRenderer.{
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
              }
            )
          ),
          actual2
        );
        let (rendered2f, _) as actual2f =
          ReasonReact.RenderedElement.flushPendingUpdates(rendered2);
        assertFlushUpdate(
          TestComponents.(
            [<ChangeCounter id=1 label="updatedText" counter=2011 />],
            [
              UpdateInstance({
                stateChanged: true,
                subTreeChanged: `NoChange,
                oldInstance:
                  <ChangeCounter id=1 label="updatedText" counter=11 />,
                newInstance:
                  <ChangeCounter id=1 label="updatedText" counter=2011 />
              })
            ]
          ),
          actual2f
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
        let updatedReactElement =
          <ButtonWrapperWrapper wrappedText="updatedText" />;
        let (rendered3, _) as actual3 =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered2f_mem,
            updatedReactElement
          );
        let previousReactElement = updatedReactElement;
        assertUpdate(
          ~label="Updating components: ChangeCounter to ButtonWrapperWrapper",
          TestComponents.(
            [
              <ButtonWrapperWrapper id=2 nestedText="wrappedText:updatedText" />
            ],
            Some(
              TestRenderer.{
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
                        <ChangeCounter id=1 label="updatedText" counter=2011 />,
                      newInstance:
                        <ButtonWrapperWrapper
                          id=2
                          nestedText="wrappedText:updatedText"
                        />
                    })
                  ])
              }
            )
          ),
          actual3
        );
        let updatedReactElement =
          <ButtonWrapperWrapper wrappedText="updatedTextmodified" />;
        let (rendered4, _) as actual4 =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered3,
            updatedReactElement
          );
        assertUpdate(
          ~label="Updating text in the button wrapper",
          TestComponents.(
            [
              <ButtonWrapperWrapper
                id=2
                nestedText="wrappedText:updatedTextmodified"
              />
            ],
            Some(
              TestRenderer.{
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
                          <Text id=5 title="wrappedText:updatedTextmodified" />
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
              }
            )
          ),
          actual4
        );
        check(
          Alcotest.bool,
          "Memoized nested button wrapper",
          true,
          ReasonReact.(
            switch (rendered3, rendered4) {
            | (
                IFlat(
                  Instance({
                    instanceSubTree:
                      IFlat(
                        Instance({
                          instanceSubTree: INested(_, [_, _, IFlat(x)])
                        })
                      )
                  })
                ),
                IFlat(
                  Instance({
                    instanceSubTree:
                      IFlat(
                        Instance({
                          instanceSubTree: INested(_, [_, _, IFlat(y)])
                        })
                      )
                  })
                )
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
        let (rendered1, _) as actual1 =
          RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) as actual2 =
          RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) as actual3 =
          RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial BoxList",
             [TestComponents.(<BoxList id=1 />)]
           );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual1
        );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual2
        );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual3
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
        let (rendered1, _) as actual1 =
          RenderedElement.flushPendingUpdates(rendered0);
        RemoteAction.act(rAction, ~action=BoxList.Create("World"));
        let (rendered2, _) as actual2 =
          RenderedElement.flushPendingUpdates(rendered1);
        RemoteAction.act(rAction, ~action=BoxList.Reverse);
        let (rendered3, _) as actual3 =
          RenderedElement.flushPendingUpdates(rendered2);
        TestRenderer.convertElement(rendered0)
        |> Assert.check(
             renderedElement,
             "Initial BoxList",
             [TestComponents.(<BoxList id=1 />)]
           );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual1
        );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual2
        );
        assertFlushUpdate(
          ~label="Add Hello then Flush",
          TestComponents.(
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
          ),
          actual3
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
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial Box",
             [TestComponents.(<BoxWithDynamicKeys id=1 state="box to move" />)]
           );
        let updatedReactElement =
          Nested("div", [stringToElement("before"), Nested("div", [box_])]);
        let (rendered1, _) as actual1 =
          RenderedElement.update(
            ~previousReactElement=box_,
            ~renderedElement=rendered0,
            updatedReactElement
          );
        assertUpdate(
          ~label="After update",
          TestComponents.(
            [
              <Text id=2 title="before" />,
              <BoxWithDynamicKeys id=1 state="box to move" />
            ],
            Some(
              TestRenderer.{
                subtreeChange:
                  `ReplaceElements((
                    [<BoxWithDynamicKeys id=1 state="box to move" />],
                    [
                      <Text id=2 title="before" />,
                      <BoxWithDynamicKeys id=1 state="box to move" />
                    ]
                  )),
                updateLog: ref([])
              }
            )
          ),
          actual1
        );
        /* TODO Compare rendered0 and rendered1 */
        /* check(
             Alcotest.bool,
             "Memoized nested box",
             true,
             ReasonReact.(
               switch (rendered0, rendered1) {
               | (
                   IFlat([Instance({instanceSubTree: IFlat([x])})]),
                   IFlat([
                     Instance({instanceSubTree: INested(_, [_, IFlat([y])])})
                   ])
                 ) =>
                 x === y
               | _ => false
               }
             )
           ); */
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
        let previousReactElement =
          ReasonReact.listToElement([
            <Box key=key1 title="Box1unchanged" />,
            <Box key=key2 title="Box2unchanged" />
          ]);
        let rendered0 = RenderedElement.render(previousReactElement);
        TestRenderer.convertElement(rendered0)
        |> check(
             renderedElement,
             "Initial Boxes",
             TestComponents.[
               <Box id=key1 state="Box1unchanged" />,
               <Box id=key2 state="Box2unchanged" />
             ]
           );
        let updatedReactElement =
          ReasonReact.listToElement([
            <Box key=key2 title="Box2changed" />,
            <Box key=key1 title="Box1changed" />
          ]);
        let previousReactElement = updatedReactElement;
        let (rendered1, _) as actual1 =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered0,
            updatedReactElement
          );
        assertUpdate(
          ~label="Swap Boxes",
          TestComponents.(
            [
              <Box id=key2 state="Box2changed" />,
              <Box id=key1 state="Box1changed" />
            ],
            Some(
              TestRenderer.{
                subtreeChange: `Nested,
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
          ),
          actual1
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
            subtree: [TestComponents.(<Text id=2 title=text />)]
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
    ),
    (
      "Test flat update",
      `Quick,
      () => {
        open ReasonReact;
        GlobalState.reset();
        let previousReactElement = <Text key=1 title="x" />;
        let rendered = RenderedElement.render(previousReactElement);
        let updatedReactElement = <Text key=2 title="y" />;
        let actual =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered,
            updatedReactElement
          );
        TestComponents.(
          assertUpdate(
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
            ),
            actual
          )
        );
      }
    ),
    (
      "Test no change",
      `Quick,
      () => {
        open ReasonReact;
        open TestComponents;
        GlobalState.reset();
        let key1 = Key.create();
        let key2 = Key.create();
        let previousReactElement =
          listToElement(
            Components.[
              <Text key=key1 title="x" />,
              <Text key=key2 title="y" />
            ]
          );
        let rendered0 = RenderedElement.render(previousReactElement);
        Assert.assertElement(
          [<Text id=1 title="x" />, <Text id=2 title="y" />],
          rendered0
        );
        let updatedReactElement =
          listToElement(
            Components.[
              <Text key=key1 title="x" />,
              <Text key=key2 title="y" />
            ]
          );
        let actual =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=rendered0,
            updatedReactElement
          );
        let previousReactElement = updatedReactElement;
        assertUpdate(
          ~label=
            "Updates the state because the state is updated with a new instance of string",
          (
            [<Text id=1 title="x" />, <Text id=2 title="y" />],
            Some({
              subtreeChange: `Nested,
              updateLog:
                ref(
                  TestRenderer.[
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
                  ]
                )
            })
          ),
          actual
        );
        let updatedReactElement =
          Components.(
            listToElement([
              <Text key=key2 title="y" />,
              <Text key=key1 title="x" />
            ])
          );
        let actual2 =
          RenderedElement.update(
            ~previousReactElement,
            ~renderedElement=fst(actual),
            updatedReactElement
          );
        assertUpdate(
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
          ),
          actual2
        );
      }
    )
  ];

Alcotest.run(~argv=[|"--verbose --color"|], "Tests", [("BoxWrapper", suite)]);
