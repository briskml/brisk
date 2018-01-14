let otherComponent = React.reducerComponent("Other");

external registerLoop : (unit => unit) => unit = "CA_registerLoop";

module View = {
  let createElement = (~layout, ~style, ~borderColor, ~children, ()) =>
    React.(
      nativeElement(
        View.make(~layout, ~style, ~borderColor, listToElement(children))
      )
    );
};

module Button = {
  let createElement = (~style, ~text, ~callback=?, ~children, ()) =>
    React.(
      nativeElement(
        Button.make(~text, ~style, ~callback?, listToElement(children))
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
        <View
          layout=React.Layout.defaultStyle
          style={
            borderWidth: 1.,
            backgroundColor: {red: 0.3, green: 0.5, blue: 0.3, alpha: 1.}
          }
          borderColor={red: 0., green: 1., blue: 0., alpha: 1.}>
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
          layout=React.Layout.{
                   ...defaultStyle,
                   paddingTop: 80,
                   paddingBottom: 80
                 }
          style={
            borderWidth: 1.,
            backgroundColor: {red: 0.3, green: 0.2, blue: 0.1, alpha: 1.}
          }
          borderColor={red: 0., green: 0., blue: 1., alpha: 1.}>
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
