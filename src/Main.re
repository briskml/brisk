let otherComponent = React.statelessComponent("Other");

let other = () => {
  ...otherComponent,
  render: (_) =>
    React.(
      nativeElement(
        View.make(
          ~x=0.,
          ~y=0.,
          ~width=300.,
          ~height=300.,
          listToElement([
            nativeElement(
              View.make(
                ~x=20.,
                ~y=20.,
                ~width=100.,
                ~height=100.,
                listToElement([])
              )
            ),
            nativeElement(
              View.make(
                ~x=140.,
                ~y=20.,
                ~width=100.,
                ~height=100.,
                listToElement([])
              )
            )
          ])
        )
      )
    )
};

let component = React.statelessComponent("My component");

let make = () => {...component, render: (_) => React.element(other())};

let rendered = React.RenderedElement.render(React.element(make()));

let view = React.mountRenderedTree(rendered);

let r = React.displayLayoutElements(view);

List.iter((view) => ignore(React.NativeView.addToWindow(view)), r);
