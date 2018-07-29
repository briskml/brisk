open Brisk_cocoa;
open Cocoa;
open React_Components;

module View = {
  let createElement = (~layout, ~style, ~children, ()) =>
    React.(element(View.make(~layout, ~style, listToElement(children))));
};

let performLayout = (root: React.NativeCocoa.hostView) => {
  React.Layout.(
    layoutNode(
      root.layoutNode,
      Flex.FixedEncoding.cssUndefined,
      Flex.FixedEncoding.cssUndefined,
      Ltr,
    )
  );
  let layout = root.layoutNode.layout;

  NSView.setFrame(
    root.view,
    layout.left |> float_of_int,
    layout.top |> float_of_int,
    layout.width |> float_of_int,
    layout.height |> float_of_int,
  );
};

let render = element => {
  let app = Lazy.force(NSApplication.app);

  app#applicationWillFinishLaunching(_ => log("app will finish"));

  app#applicationDidFinishLaunching(_ => {
    log("app did finish");

    let root = {
      React.NativeCocoa.view: NSView.make(),
      layoutNode:
        React.Layout.LayoutSupport.createNode(
          ~withChildren=[||],
          ~andStyle={
            ...React.Layout.LayoutSupport.defaultStyle,
            width: 320,
            height: 240,
          },
          (),
        ),
    };

    let w = NSWindow.makeWithContentRect(0., 0., 680., 468.);
    w#windowDidResize(_ => performLayout(root));
    w#center;
    w#makeKeyAndOrderFront;

    w#setContentView(root.view);

    let rendered =
      ref(React.RenderedElement.render(React.element(element)));
    ();
    React.HostView.mountRenderedElement(root, rendered^);
    performLayout(root);
  });
  app#run;
};

/* module Component = {
     let otherComponent = React.reducerComponent("Other");
     let createElement = (~children, ()) => {
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
             }
           />;
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
             }
           />;
         },
     };
   }; */

module Component = {
  let otherComponent = React.statelessComponent("Other");
  let createElement = (~children, ()) => {
    ...otherComponent,
    render: ({state, reduce}) =>
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
        }
      />,
  };
};

render(<Component />);

/* React.run(_ => render()); */
