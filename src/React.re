module NativeView = {
  type t;
  type hostView = t;
  [@noalloc] external getWindow : unit => t = "View_getWindow";
  [@noalloc] external makeInstance : ([@untagged] int) => t =
    "View_newView_byte" "View_newView";
  [@noalloc] external addChild : (t, t) => t = "View_addChild";
  [@noalloc] external removeChild : (t, t) => t = "View_removeChild";
  external getInstance : int => option(t) = "View_getInstance";
  [@noalloc]
  external setFrame :
    ([@untagged] int, [@untagged] int, [@untagged] int, [@untagged] int, t) =>
    unit =
    "View_setFrame_byte" "View_setFrame";
  [@noalloc]
  external setBackgroundColor :
    (
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      t
    ) =>
    unit =
    "View_setBackgroundColor_byte" "View_setBackgroundColor";
  [@noalloc] external setBorderWidth : ([@unboxed] float, t) => unit =
    "View_setBorderWidth_byte" "View_setBorderWidth";
  [@noalloc]
  external setBorderColor :
    (
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      [@unboxed] float,
      t
    ) =>
    unit =
    "View_setBorderColor_byte" "View_setBorderColor";
  [@noalloc] external setCornerRadius : ([@unboxed] float, t) => unit =
    "View_setBorderColor_byte" "View_setBorderColor";
};

include ReactCore.Make(NativeView);

module View = {
  type t = NativeView.t;
  type color = {
    red: float,
    green: float,
    blue: float,
    alpha: float
  };
  type style = {
    backgroundColor: color,
    borderWidth: float
  };
  let component = statelessNativeComponent("View");
  let make = (~layout, ~style, ~borderColor, children) => {
    ...component,
    render: (_) => {
      make: (id) => NativeView.makeInstance(id),
      setProps: (view) => {
        let {red, green, blue, alpha} = style.backgroundColor;
        NativeView.setBackgroundColor(red, green, blue, alpha, view);
        NativeView.setBorderWidth(style.borderWidth, view);
        let {red, green, blue, alpha} = borderColor;
        NativeView.setBorderColor(red, green, blue, alpha, view)
      },
      children,
      style: layout
    }
  };
};

module Button = {
  type t = NativeView.t;
  external makeInstance : int => NativeView.t = "Button_makeInstance";
  external setText : (string, t) => t = "Button_setText";
  [@noalloc] external setCallback : (unit => unit, t) => t =
    "Button_setCallback";
  let component = statelessNativeComponent("Button");
  let make = (~text, ~style, ~callback=?, children) => {
    ...component,
    render: (_) => {
      make: (id) => {
        let instance = makeInstance(id) |> setText(text);
        switch callback {
        | Some(callback) => setCallback(callback, instance)
        | None => instance
        }
      },
      setProps: (_) => (),
      children,
      style
    }
  };
};
