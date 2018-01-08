let otherComponent = React.reducerComponent("Other");

[@noalloc] external registerLoop : (unit => unit) => unit = "CA_registerLoop";

module View = {
  let createElement = (~style=?, ~children, ()) =>
    React.(nativeElement(~style?, View.make(React.listToElement(children))));
};

module Button = {
  let createElement = (~style=?, ~text, ~callback=?, ~children, ()) =>
    React.(
      nativeElement(
        ~style?,
        Button.make(~text, ~callback?, React.listToElement(children))
      )
    );
};

let render = (element) => {
  let window = React.NativeView.getWindow();
  let rendered = React.RenderedElement.render(React.element(element));
  let outputTree = React.OutputTree.fromRenderedElement(window, rendered);
  let layout =
    React.LayoutTest.make(~root=window, ~outputTree, ~width=320, ~height=480);
  React.LayoutTest.performLayout(layout);
  React.OutputTree.mountForest(outputTree);
  registerLoop(
    () => {
      let (_, updateLog) = React.RenderedElement.flushPendingUpdates(rendered);
      ignore(React.OutputTree.applyUpdateLog(updateLog, outputTree, window));
      let layout =
        React.LayoutTest.make(
          ~root=window,
          ~outputTree,
          ~width=320,
          ~height=480
        );
      React.LayoutTest.performLayout(layout)
    }
  )
};

module Component = {
  let createElement = (~children, ()) => {
    ...otherComponent,
    initialState: (_) => false,
    reducer: (x, _) => React.Update(x),
    render: ({state, reduce}) =>
      if (state) {
        <View>
          <Button
            style=React.Layout.{...defaultStyle, height: 100}
            text="Cell one"
            callback=(reduce(() => ! state))
          />
          <Button
            style=React.Layout.{...defaultStyle, height: 100}
            text="Cell two"
            callback=(reduce(() => ! state))
          />
        </View>
      } else {
        <View
          style=React.Layout.{
                  ...defaultStyle,
                  paddingTop: 80,
                  paddingBottom: 80
                }>
          <Button
            style=React.Layout.{...defaultStyle, height: 100}
            text="well"
            callback=(reduce(() => ! state))
          />
        </View>
      }
  };
};

render(<Component />);
