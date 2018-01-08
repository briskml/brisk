let otherComponent = React.reducerComponent("Other");

module View = {
  let createElement = React.View.make;
};

let other = () => {
  ...otherComponent,
  initialState: (_) => false,
  reducer: (x, _) => React.Update(x),
  render: ({state, reduce}) =>
    if (state) {
      React.(
        nativeElement(
          View.make(
            ~x=0.,
            ~y=0.,
            ~width=300.,
            ~height=300.,
            listToElement([
              nativeElement(
                View.make(
                  ~x=20.,
                  ~y=20.,
                  ~width=100.,
                  ~height=100.,
                  listToElement([])
                )
              ),
              nativeElement(
                View.make(
                  ~x=140.,
                  ~y=20.,
                  ~width=100.,
                  ~height=100.,
                  listToElement([])
                )
              )
            ])
          )
        )
      )
    } else {
      React.(
        nativeElement(
          View.make(
            ~x=0.,
            ~y=0.,
            ~width=300.,
            ~height=300.,
            listToElement([
              nativeElement(
                Button.make(
                  ~x=140.,
                  ~y=20.,
                  ~width=100.,
                  ~height=100.,
                  ~text="HELLO",
                  ~callback=reduce(() => ! state),
                  listToElement([])
                )
              )
            ])
          )
        )
      )
    }
};

let window = React.NativeView.getWindow();

let rendered = React.RenderedElement.render(React.element(other()));

let outputTree = React.OutputTree.fromRenderedElement(window, rendered);

React.OutputTree.mountForest(outputTree);

Unix.sleep(5);

let (_, updateLog) = React.RenderedElement.flushPendingUpdates(rendered);

ignore(React.OutputTree.applyUpdateLog(updateLog, outputTree, window));
