module NativeCocoa = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement = CocoaTypes.view;

  [@deriving (show({with_path: false}), eq)]
  type node = {
    view: hostElement,
    layoutNode: Layout.LayoutSupport.LayoutTypes.node,
  };

  let instanceMap: Hashtbl.t(int, node) = Hashtbl.create(1000);

  let isDirty = ref(false);

  let markAsStale = () => {
    isDirty := true;
  };

  let beginChanges = () => ();

  let commitChanges = () => ();

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    Layout.cssNodeInsertChild(parent.layoutNode, child.layoutNode, position);
    BriskView.addSubview(parent.view, child.view);
    parent;
  };

  let deleteNode = (~parent, ~child) => {
    BriskView.removeSubview(child.view);
    parent;
  };

  let moveNode = (~parent, ~child as _, ~from as _, ~to_ as _) => parent;
};

include Brisk_reconciler.Make(NativeCocoa);

module Task = {
  external runInBackground: (unit => unit) => unit = "ml_runTaskInBackground";
};

module RunLoop = {
  let spawn = () => {
    Printexc.(get_callstack(20) |> raw_backtrace_to_string) |> print_endline;
  };

  [@noalloc]
  external getMainFileDescr: unit => [@untagged] int =
    "ml_getMainFd" "ml_getMainFd";

  let fileDescrOfInt: int => Unix.file_descr = Obj.magic;
  let intOfFileDescr: Unix.file_descr => int = Obj.magic;

  let rootRef = ref(None);
  let renderedRef = ref(None);
  let heightRef = ref(0.);

  let setWindowHeight = height => {
    heightRef := height;
    NativeCocoa.markAsStale();
  };

  let rec traverseAndApplyLayout =
          (~height, node: Layout.LayoutSupport.LayoutTypes.node) => {
    let layout = node.layout;

    let nodeTop = float_of_int(layout.top);
    let nodeHeight = layout.height |> float_of_int;
    let flippedTop = height -. nodeHeight -. nodeTop;

    BriskView.setFrame(
      node.context,
      layout.left |> float_of_int,
      flippedTop,
      layout.width |> float_of_int,
      nodeHeight,
    );

    node.children
    |> Array.iter(child => traverseAndApplyLayout(~height=nodeHeight, child));
  };

  let performLayout = (~height, root: NativeCocoa.node) => {
    let node = root.layoutNode;
    Layout.(
      layoutNode(
        node,
        Flex.FixedEncoding.cssUndefined,
        Flex.FixedEncoding.cssUndefined,
        Ltr,
      )
    );
    traverseAndApplyLayout(~height, node);
  };

  let flushPendingUpdates = () =>
    switch (renderedRef^) {
    | Some(rendered) =>
      let updated =
        rendered
        |> RenderedElement.flushPendingUpdates
        |> RenderedElement.executePendingEffects;
      renderedRef := Some(updated);
    | _ => ()
    };

  let flushAndLayout = () =>
    switch (rootRef^) {
    | Some(root) =>
      flushPendingUpdates();
      switch (renderedRef^) {
      | Some(rendered) =>
        RenderedElement.executeHostViewUpdates(rendered) |> ignore;
        performLayout(~height=heightRef^, root);
      | _ => ()
      };
    | _ => ()
    };

  let renderAndMount =
      (~height, root: NativeCocoa.node, element: syntheticElement) => {
    let rendered = RenderedElement.render(root, element);
    rootRef := Some(root);
    renderedRef := Some(rendered);
    heightRef := height;
    RenderedElement.executeHostViewUpdates(rendered) |> ignore;
    performLayout(~height, root);
  };

  [@noalloc]
  external shouldReleaseRuntime: unit => [@untagged] int =
    "ml_shouldReleaseRuntime" "ml_shouldReleaseRuntime";

  [@noalloc]
  external scheduleHostViewUpdateAndLayout: ([@untagged] int) => unit =
    "ml_scheduleHostViewUpdateAndLayout" "ml_scheduleHostViewUpdateAndLayout";

  let scheduleHostViewUpdateAndLayout = () => {
    open Lwt.Infix;
    let (fd_out, fd_in) = Lwt_unix.pipe();

    let fd = fd_in |> Lwt_unix.unix_file_descr |> intOfFileDescr;

    scheduleHostViewUpdateAndLayout(fd);

    Lwt_unix.read(fd_out, Bytes.create(1), 0, 1) >|= (_ => ());
  };

  let rec run = () => {
    let fd =
      getMainFileDescr() |> fileDescrOfInt |> Lwt_unix.of_unix_file_descr;
    let _ = Lwt_unix.read(fd, Bytes.create(1), 0, 1);
    Lwt.wakeup_paused();
    Lwt_engine.iter(true);
    Lwt.wakeup_paused();

    if (shouldReleaseRuntime() < 1) {
      print_endline("ml: shouldReleaseRuntime 0");
      if (NativeCocoa.isDirty^) {
        Lwt.Infix.(
          scheduleHostViewUpdateAndLayout()
          >|= (() => print_endline("ok"))
          |> ignore
        );
      } else {
        flushPendingUpdates();
        run();
      };
    } else {
      print_endline("ml: shouldReleaseRuntime 1");
      if (NativeCocoa.isDirty^) {
        Lwt.Infix.(
          scheduleHostViewUpdateAndLayout()
          >|= (() => print_endline("ok"))
          |> ignore
        );
      }
    };
  };

  /* Callback.register(
              "Brisk_RunLoop_updateHostViewAndLayout",
              flushAndLayout); */
};
