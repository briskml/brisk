open Brisk;

let component = nativeComponent("activityIndicator");

type attribute = [ Layout.style];
type style = list(attribute);

let measure = (node, _width, _widthMode, _height, _heightMode) => {
  Layout.FlexLayout.LayoutSupport.LayoutTypes.{
    width: BriskProgressIndicator.fittingWidth(node.context.view),
    height: BriskProgressIndicator.fittingHeight(node.context.view),
  };
};

let activityIndicator = (~style: style=[], ~children as _: list(unit), ()) =>
  component(hooks =>
    (
      hooks,
      {
        make: () => {
          let view = BriskProgressIndicator.make();
          let layoutNode =
            Layout.Node.make(~style, ~measure, {view, isYAxisFlipped: false});
          {view, layoutNode};
        },
        configureInstance: (~isFirstRender as _, node) => {
          node;
        },
        children: Brisk.empty,
      },
    )
  );
