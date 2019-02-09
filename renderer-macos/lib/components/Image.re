open Brisk;
open Lwt_result.Infix;

type attribute = [ Layout.style | Styles.viewStyle];

type style = list(attribute);

let component = nativeComponent("Image");

let measure = (node, _, _, _, _) => {
  open Layout.FlexLayout.LayoutSupport.LayoutTypes;

  let {context: img}: node = node;

  let width = BriskImage.getImageWidth(img) |> int_of_float;
  let height = BriskImage.getImageHeight(img) |> int_of_float;

  {width, height};
};

let getImage = (~url: string) => {
  switch (
    Httpkit_lwt.Client.(
      Httpkit.Client.Request.create(
        ~headers=[("User-Agent", "Reason HttpKit")],
        `GET,
        Uri.of_string(url),
      )
      |> Https.send
      >>= Response.body
      |> Lwt_main.run
    )
  ) {
  | exception _e => "Error"
  | Ok(body) =>
    /* get image instead of random string */
    Yojson.Basic.(from_string(body) |> Util.member("login") |> to_string)
  | Error(_) => "error"
  };
};

let make = (~style=[], ~source, ~url, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskImage.make(~source, ());

        /* TODO: handle user image */
        print_string(getImage(~url));

        {view, layoutNode: Layout.Node.make(~measure, ~style, view)};
      },
      configureInstance: (~isFirstRender as _, {view} as node) => {
        style
        |> List.iter(attribute =>
             switch (attribute) {
             | #Styles.viewStyle => Styles.setViewStyle(view, attribute)
             | #Layout.style => ()
             }
           );
        node;
      },
      children,
    }
  );

let createElement = (~style=[], ~source, ~url, ~children, ()) =>
  element(make(~style, ~source, ~url, listToElement(children)));