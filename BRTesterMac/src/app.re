open Brisk_cocoa;
open Cocoa;
open Lwt.Infix;

[@noalloc] external lwt_start: unit => unit = "ml_lwt_iter";

module Component = {
  let otherComponent = React.reducerComponent("Other");
  let createElement = (~children as _, ()) => {
    ...otherComponent,
    initialState: _ => None,
    reducer: (x, _) => React.Update(x),
    render: ({state, reduce}) =>
      switch (state) {
      | Some(code) =>
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
            title={Cohttp.Code.string_of_status(code)}
            callback={reduce(() => None)}
          />
          <Button
            layout={
              ...Layout.LayoutSupport.defaultStyle,
              width: 100,
              height: 100,
            }
            title="Cell two"
            callback={reduce(() => None)}
          />
        </View>
      | None =>
        <View
          layout=Layout.LayoutSupport.{
            ...defaultStyle,
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
              width: 600,
              height: 40,
            }
            title="Youre gonna have to wait a bit"
            callback={
                       let callback = reduce(code => code);
                       (
                         () => {
                           lwt_start();
                           print_endline("Started");
                           ignore(
                             Cohttp_lwt_unix.Client.get(
                               Uri.of_string("http://example.com"),
                             )
                             >>= (
                               ((response, _body)) => {
                                 print_endline("Finished");
                                 Lwt.return(
                                   callback(
                                     Some(Cohttp.Response.status(response)),
                                   ),
                                 );
                               }
                             ),
                           );
                         }
                       );
                     }
          />
        </View>
      },
  };
};

let lwt_iter = () => {
  print_endline("iter");
  Lwt.wakeup_paused ();
  Lwt_engine.iter(Lwt.paused_count () == 0);
  Lwt.wakeup_paused ();
  print_endline("iter finish");
}

let () = {
  Callback.register("Brisk.flush", React.RunLoop.loop);
  Callback.register("Brisk_lwt_iter", lwt_iter);
  let app = Lazy.force(NSApplication.app);

  let appName = "BriskMac";

  app#applicationWillFinishLaunching(_ => {
    log("app will finish");
    let menu = Menu.makeMainMenu(appName);
    CocoaMenu.NSMenu.add(~kind=Main, menu);
  });

  app#applicationDidFinishLaunching(_ => {
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
      React.element(<Component />),
    );
  });
};
