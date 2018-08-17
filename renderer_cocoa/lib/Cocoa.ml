(**
   Cocoa bindings for OCaml.
   Original project by Nicolás Ojeda Bär © https://github.com/nojb/ocaml-cocoa
*)

module T =
  Ephemeron.K1.Make (struct
    type t = int

    let equal = (=)
    let hash = Hashtbl.hash
  end)

external _NSLog: string -> unit = "ml_NSLog"

let log fmt =
  Printf.ksprintf _NSLog fmt

type nsApp

type nsWindow

type nsView

module NSApplication = struct
  external _NSApplication_NSApp: int -> nsApp = "ml_NSApplication_NSApp" "ml_NSApplication_NSApp"
  external _NSApplication_run: nsApp -> unit = "ml_NSApplication_run" "ml_NSApplication_run"

  class type t =
    object
      method run: unit

      method applicationWillFinishLaunching: (unit -> unit) -> unit
      method applicationDidFinishLaunching: (unit -> unit) -> unit
    end

  type applicationDelegate =
    {
      mutable applicationWillFinishLaunching: (unit -> unit);
      mutable applicationDidFinishLaunching: (unit -> unit);
    }

  let delegate () =
    {
      applicationWillFinishLaunching = (fun () -> ());
      applicationDidFinishLaunching = (fun () -> ());
    }

  let application_table = T.create 1
  let app_id = ref 0

  let app =
    lazy begin
      let id = incr app_id; !app_id in
      let app = _NSApplication_NSApp id in
      let del = delegate () in
      T.add application_table id del;
      object
        method run = _NSApplication_run app

        method applicationWillFinishLaunching f =
          del.applicationWillFinishLaunching <- f
        method applicationDidFinishLaunching f =
          del.applicationDidFinishLaunching <- f
      end
    end

  type delegateSelector =
    | ApplicationWillFinishLaunching
    | ApplicationDidFinishLaunching

  let applicationDelegate id sel =
    try
      let del = T.find application_table id in
      match sel with
      | ApplicationWillFinishLaunching ->
          del.applicationWillFinishLaunching ()
      | ApplicationDidFinishLaunching ->
          del.applicationDidFinishLaunching ()
    with Not_found ->
      ()

  let () =
    Callback.register "NSApp.delegate" applicationDelegate
end

module NSWindow = struct
  external _NSWindow_makeWithContentRect: int -> float -> float -> float -> float -> nsWindow = "ml_NSWindow_makeWithContentRect"

  external _NSWindow_isVisible: nsWindow -> bool = "ml_NSWindow_isVisible"
  external _NSWindow_center: nsWindow -> unit = "ml_NSWindow_center"
  external _NSWindow_makeKeyAndOrderFront:
    nsWindow -> unit = "ml_NSWindow_makeKeyAndOrderFront"
  external _NSWindow_setTitle: nsWindow -> string -> unit = "ml_NSWindow_setTitle"
  external _NSWindow_title: nsWindow -> string = "ml_NSWindow_title"
  external _NSWindow_contentView: nsWindow -> nsView = "ml_NSWindow_contentView"
  external _NSWindow_setContentView: nsWindow -> nsView -> unit = "ml_NSWindow_setContentView"

  class type t =
    object
      method isVisible: bool
      method center: unit
      method makeKeyAndOrderFront: unit
      method setTitle: string -> unit
      method title: string

      method contentView: nsView
      method setContentView: nsView -> unit

      method windowDidResize: (unit -> unit) -> unit
    end

  type windowDelegate = {
    mutable windowDidResize: (unit -> unit);
  }

  let delegate () = {
    windowDidResize = (fun () -> ());
  }

  let win_id = ref 0
  let window_table = T.create 0

  let makeWithContentRect x y w h =
    let id = incr win_id; !win_id in
    let win = _NSWindow_makeWithContentRect id x y w h in
    let del = delegate () in
    T.add window_table id del;
    object
      method isVisible = _NSWindow_isVisible win
      method center = _NSWindow_center win
      method makeKeyAndOrderFront = _NSWindow_makeKeyAndOrderFront win
      method setTitle s = _NSWindow_setTitle win s
      method title = _NSWindow_title win

      method contentView = _NSWindow_contentView win
      method setContentView v = _NSWindow_setContentView win v

      method windowDidResize f =
        del.windowDidResize <- f
    end

  type windowDelegateSelector =
    | WindowDidResize

  let windowDelegate id sel =
    try
      let del = T.find window_table id in
      match sel with
      | WindowDidResize ->
          del.windowDidResize ()
    with Not_found ->
      Printf.ksprintf prerr_endline "Window #%d has been GCed" id

  let () =
    Callback.register "NSWindow.delegate" windowDelegate
end

module NSView = struct
  type t = nsView

  external _NSView_make: unit -> t = "ml_NSView_make"
  external _NSView_memoize: int -> t -> unit = "ml_NSView_memoize"

  external _NSView_free: int -> unit = "ml_NSView_free"

  external _NSView_setBorderWidth: t -> float -> unit = "ml_NSView_setBorderWidth"

  external _NSView_setBorderColor: t -> float -> float -> float -> float -> unit =
    "ml_NSView_setBorderColor"

  external _NSView_setBackgroundColor: t -> float -> float -> float -> float -> unit =
    "ml_NSView_setBackgroundColor"

  external _NSView_addSubview: t -> t -> t = "ml_NSView_addSubview"

  (* external _NSView_insertSubviewAt: t -> t -> int -> unit =
     "ml_NSView_insertSubviewAt" *)

  external _NSView_setFrame: t -> float -> float -> float -> float -> unit = "ml_NSView_setFrame"

  let make = _NSView_make

  let memoize = _NSView_memoize
  let free = _NSView_free

  let setBorderWidth = _NSView_setBorderWidth
  let setBorderColor = _NSView_setBorderColor
  let setBackgroundColor = _NSView_setBackgroundColor

  let setFrame view = _NSView_setFrame view

  let addSubview view child _position =
    _NSView_addSubview view child
end

module NSButton = struct
  type t = NSView.t

  external _NSButton_make: unit -> t = "ml_NSButton_make"
  external _NSButton_memoize: int -> t -> unit = "ml_NSButton_memoize"
  external _NSButton_free: int -> unit = "ml_NSButton_free"

  external _NSButton_setFrame: t -> float -> float -> float -> float -> unit =
    "ml_NSButton_setFrame"

  external _NSButton_setTitle: t -> string -> t = "ml_NSButton_setTitle"
  external _NSButton_setCallback: t -> (unit -> unit) -> t = "ml_NSButton_setCallback"

  let make = _NSButton_make
  let memoize id btn = _NSButton_memoize id btn
  let free = _NSButton_free

  (* let setBorderWidth view = _NSView_setBorderWidth view
     let setBorderColor view = _NSView_setBorderColor view
     let setBackgroundColor view = _NSView_setBackgroundColor view *)

  let setFrame = _NSButton_setFrame

  let setTitle = _NSButton_setTitle
  let setCallback = _NSButton_setCallback

end
