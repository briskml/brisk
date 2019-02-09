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

let getUserDate = (id: int) => {
  let https_url = "https://api.github.com/user/" ++ string_of_int(id);
  /* Logs.app(m => m("Requesting: %s", https_url)); */

  switch (
    Httpkit_lwt.Client.(
      Httpkit.Client.Request.create(
        ~headers=[("User-Agent", "Reason HttpKit")],
        `GET,
        https_url |> Uri.of_string,
      )
      |> Https.send
      >>= Response.body
      |> Lwt_main.run
    )
  ) {
  | exception _e =>
    /* Logs.err(m => m("%s", Printexc.to_string(e))); */
    "asdasd"
  | Ok(body) =>
    let json = Yojson.Basic.from_string(body);
    open Yojson.Basic.Util;
    let login = json |> member("login") |> to_string;
    /* let id = to_int(member("id", json)); */

    /* Logs.app(m => m("login: %s", login)); */
    /* Logs.app(m => m("id: %d", id)); */

    /* Logs.app(m => m("Response: %s", body)); */
    login;
  | Error(_) =>
    Logs.err(m => m("Something went wrong!!!"));
    "error";
  };
};

let make = (~style=[], ~source, ~url, children) =>
  component((_: Hooks.empty) =>
    {
      make: () => {
        let view = BriskImage.make(~source, ());

        print_string(url);

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