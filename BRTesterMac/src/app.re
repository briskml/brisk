open Brisk_cocoa;
open Cocoa;

let render = element => {
  let app = Lazy.force(NSApplication.app);

  let appName = "BriskMac";

  app#applicationWillFinishLaunching(_ => {
    log("app will finish");
    let menu = Menu.makeMainMenu(appName);
    CocoaMenu.NSMenu.add(~kind=Main, menu);
  });

  app#applicationDidFinishLaunching(_ => {
    log("app did finish");

    let window = NSWindow.makeWithContentRect(0., 0., 680., 468.);

    let root = {
      let view = NSView.make();
      {
        React.NativeCocoa.view,
        layoutNode:
          Layout.LayoutSupport.createNode(
            ~withChildren=[||],
            ~andStyle={
              ...Layout.LayoutSupport.defaultStyle,
              width: 400,
              height: 460,
            },
            view,
          ),
      };
    };

    window#center;
    window#makeKeyAndOrderFront;
    window#setTitle(appName);
    window#setContentView(root.view);

    window#windowDidResize(_ =>
      React.RunLoop.setWindowHeight(window#contentHeight)
    );

    React.RunLoop.run(
      ~height=window#contentHeight,
      root,
      React.element(element),
    );
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
            ...Layout.LayoutSupport.defaultStyle,
            width: 100,
            height: 100,
          }
          style=[
            `background(`rgb((0, 255, 0))),
            `borderColor(`rgb((0, 0, 255))),
            `borderWidth(`pt(1.)),
          ]>
          <Button
            layout={
              ...Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell one"
            callback={reduce(() => !state)}
          />
          <Button
            layout={
              ...Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell two"
            callback={reduce(() => !state)}
          />
        </View>;
      } else {
        <View
          layout=Layout.LayoutSupport.{
            ...defaultStyle,
            width: 200,
            height: 400,
          }
          style=[
            `background(`rgb((255, 0, 0))),
            `borderColor(`rgb((0, 255, 0))),
            `borderWidth(`pt(1.)),
          ]>
          <Button
            layout=Layout.LayoutSupport.{
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
