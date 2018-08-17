open Brisk_cocoa;
open Cocoa;
open React_Components;

module View = {
  let createElement = (~layout, ~style, ~children, ()) =>
    React.(element(View.make(~layout, ~style, listToElement(children))));
};

module Button = {
  let createElement =
      (~layout, ~style=?, ~title=?, ~callback=?, ~children, ()) =>
    React.(
      element(
        Button.make(
          ~layout,
          ~style?,
          ~title?,
          ~callback?,
          listToElement(children),
        ),
      )
    );
};

let render = element => {
  let app = Lazy.force(NSApplication.app);

  app#applicationWillFinishLaunching(_ => log("app will finish"));

  app#applicationDidFinishLaunching(_ => {
    log("app did finish");

    let w = NSWindow.makeWithContentRect(0., 0., 680., 468.);

    let root = {
      React.NativeCocoa.view: NSView.make(),
      layoutNode:
        React.Layout.LayoutSupport.createNode(
          ~withChildren=[||],
          ~andStyle={
            ...React.Layout.LayoutSupport.defaultStyle,
            width: 320,
            height: 460,
          },
          (),
        ),
    };

    w#windowDidResize(_ => React.RunLoop.performLayout(root));
    w#center;
    w#makeKeyAndOrderFront;

    w#setContentView(root.view);

    let rendered = React.RenderedElement.render(React.element(element));
    React.HostView.mountRenderedElement(root, rendered);
    React.RunLoop.performLayout(root);
    /* React.RunLoop.run(root, React.element(element)); */
  });
  app#run;
};

module Component = {
  let otherComponent = React.reducerComponent("Other");
  let createElement = (~children as _, ()) => {
    ...otherComponent,
    initialState: _ => false,
    reducer: (x, _) => React.Update(x),
    render: ({state, reduce}) =>
      if (state) {
        <View
          layout={
            ...React.Layout.LayoutSupport.defaultStyle,
            width: 100,
            height: 100,
          }
          style={
            borderWidth: 1.,
            backgroundColor: {
              red: 0.3,
              green: 0.5,
              blue: 0.3,
              alpha: 1.,
            },
            borderColor: {
              red: 0.,
              green: 1.,
              blue: 0.,
              alpha: 1.,
            },
          }>
          <Button
            layout={
              ...React.Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell one"
            callback=(reduce(() => !state))
          />
          <Button
            layout={
              ...React.Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell two"
            callback=(reduce(() => !state))
          />
        </View>;
      } else {
        <View
          layout=React.Layout.LayoutSupport.{
            ...defaultStyle,
            width: 100,
            height: 100,
            paddingTop: 80,
            paddingBottom: 80,
          }
          style={
            borderWidth: 1.,
            backgroundColor: {
              red: 0.3,
              green: 0.2,
              blue: 0.1,
              alpha: 1.,
            },
            borderColor: {
              red: 0.,
              green: 0.,
              blue: 1.,
              alpha: 1.,
            },
          }>
          <Button
            layout=React.Layout.LayoutSupport.{
              ...defaultStyle,
              width: 100,
              height: 100,
            }
            title="well"
            callback=(reduce(() => !state))
          />
        </View>;
      },
  };
};

Callback.register("React.run", _ => render(<Component />));
