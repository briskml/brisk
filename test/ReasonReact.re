open R2n2;

module Implementation = {
  let map = Hashtbl.create(1000);
  type hostView =
    | Text(string)
    | View;
  let getInstance = id =>
    if (Hashtbl.mem(map, id)) {
      Some(Hashtbl.find(map, id));
    } else {
      None;
    };
};

include ReactCore_Internal.Make(Implementation);

module Text = {
  /* FIXME: If a different prop is supplied as title, the change is not picked up by React. It's because make returns a host element and there's no way to know if a Host element is not changed. */
  let component = statefulNativeComponent("Text");
  let make = (~title="ImABox", _children) => {
    ...component,
    initialState: () => title,
    willReceiveProps: (_) => title,
    printState: (_) => title,
    render: (_) => {
      children: listToElement([]),
      make: id => {
        let elem = Implementation.Text(title);
        Hashtbl.add(Implementation.map, id, elem);
        elem;
      },
      updateInstance: (_) => ()
    }
  };
  let createElement = (~key=?, ~title=?, ~children as _children, ()) =>
    element(~key?, make(~title?, ()));
};

let stringToElement = string => <Text title=string />;
