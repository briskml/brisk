module Create = (Node: Flex.Spec.Node, Encoding: Flex.Spec.Encoding) => {
  include Flex.Layout.Create(Node, Encoding);
  include Style.Create(Encoding);

  module Color = Color0;

  let isUndefined = Encoding.isUndefined;

  let makeLayoutNode = (~style, hostView: Node.context) => {
    let int_of_scalar = s => s |> Encoding.scalarToFloat |> int_of_float;
    let andStyle = LayoutSupport.defaultStyle;
    List.iter(
      attr =>
        switch (attr) {
        | `width(w) => andStyle.width = int_of_scalar(w)
        | `height(h) => andStyle.height = int_of_scalar(h)
        | `border(({width, _}: Border.t)) =>
          if (!isUndefined(width)) {
            andStyle.border = int_of_scalar(width);
          }
        | `padding(l, t, r, b) =>
          if (!isUndefined(l)) {
            andStyle.paddingLeft = int_of_scalar(l);
          };
          if (!isUndefined(t)) {
            andStyle.paddingTop = int_of_scalar(t);
          };
          if (!isUndefined(r)) {
            andStyle.paddingRight = int_of_scalar(r);
          };
          if (!isUndefined(b)) {
            andStyle.paddingBottom = int_of_scalar(b);
          };
        | `margin(l, t, r, b) =>
          if (!isUndefined(l)) {
            andStyle.marginLeft = int_of_scalar(l);
          };
          if (!isUndefined(t)) {
            andStyle.marginTop = int_of_scalar(t);
          };
          if (!isUndefined(r)) {
            andStyle.marginRight = int_of_scalar(r);
          };
          if (!isUndefined(b)) {
            andStyle.marginBottom = int_of_scalar(b);
          };
        | _ => ignore()
        },
      style,
    );
    LayoutSupport.createNode(~withChildren=[||], ~andStyle, hostView);
  };

  let cssNodeInsertChild = (node, child, index) => {
    open LayoutSupport;
    open LayoutTypes;
    assert(child.parent === theNullNode);
    assert(node.measure === None);
    let capacity = Array.length(node.children);
    if (capacity == node.childrenCount) {
      /* TODO:Simply use Array.fill (no need to allocate a separate `fill` array
       * */
      let fill = Array.make(capacity + 1, theNullNode);
      Array.blit(node.children, 0, fill, 0, capacity);
      node.children = fill;
    };
    for (i in node.childrenCount downto index + 1) {
      node.children[i] = node.children[i - 1];
    };
    node.childrenCount = node.childrenCount + 1;
    node.children[index] = child;
    child.parent = node;

    LayoutSupport.markDirtyInternal(node);
  };
};
