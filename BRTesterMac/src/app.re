open Brisk_cocoa;
open Cocoa;
open React_Components;

let render = element => {
  let app = Lazy.force(NSApplication.app);

  app#applicationWillFinishLaunching(_ => log("app will finish"));

  app#applicationDidFinishLaunching(_ => {
    log("app did finish");

    let w = NSWindow.makeWithContentRect(0., 0., 680., 468.);

    let root = {
      let view = NSView.make();
      {
        React.NativeCocoa.view,
        layoutNode:
          React.Layout.LayoutSupport.createNode(
            ~withChildren=[||],
            ~andStyle={
              ...React.Layout.LayoutSupport.defaultStyle,
              width: 400,
              height: 460,
            },
            view,
          ),
      };
    };

    w#center;
    w#makeKeyAndOrderFront;
    w#setTitle("BriskMac");
    w#setContentView(root.view);

    w#windowDidResize(_ => React.RunLoop.performLayout(root));

    React.RunLoop.run(root, React.element(element));
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
              red: 0.,
              green: 255.,
              blue: 0.,
              alpha: 1.,
            },
            borderColor: {
              red: 0.,
              green: 0.,
              blue: 255.,
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
            callback={reduce(() => !state)}
          />
          <Button
            layout={
              ...React.Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell two"
            callback={reduce(() => !state)}
          />
        </View>;
      } else {
        <View
          layout=React.Layout.LayoutSupport.{
            ...defaultStyle,
            width: 200,
            height: 400,
          }
          style={
            borderWidth: 1.,
            backgroundColor: {
              red: 255.,
              green: 0.,
              blue: 0.,
              alpha: 1.,
            },
            borderColor: {
              red: 0.,
              green: 255.,
              blue: 0.,
              alpha: 1.,
            },
          }>
          <Button
            layout=React.Layout.LayoutSupport.{
              ...defaultStyle,
              width: 100,
              height: 40,
            }
            title="Well"
            callback={reduce(() => !state)}
          />
        </View>;
      },
  };
};

Callback.register("React.run", _ => render(<Component />));
