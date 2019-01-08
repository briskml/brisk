module Create = (Node: Flex.Spec.Node, Encoding: Flex.Spec.Encoding) => {
  include Flex.Layout.Create(Node, Encoding);
  include Style.Create(Encoding);

  module Color = Color0;

  open Encoding;
  open LayoutSupport;
  open LayoutTypes;

  let flexDirection = d =>
    `flexDirection(
      switch (d) {
      | `column => Column
      | `columnReverse => ColumnReverse
      | `row => Row
      | `rowReverse => RowReverse
      },
    );

  let convertAlign =
    fun
    | `auto => AlignAuto
    | `flexStart => AlignFlexStart
    | `center => AlignCenter
    | `flexEnd => AlignFlexEnd
    | `stretch => AlignStretch;

  let justifyContent = j =>
    `justifyContent(
      switch (j) {
      | `flexStart => JustifyFlexStart
      | `center => JustifyCenter
      | `flexEnd => JustifyFlexEnd
      | `spaceBetween => JustifySpaceBetween
      | `spaceAround => JustifySpaceAround
      },
    );

  let alignContent = a => `alignContent(convertAlign(a));
  let alignItems = a => `alignItems(convertAlign(a));
  let alignSelf = a => `alignSelf(convertAlign(a));

  type style = [
    | `position(Position.t)
    | `flexDirection(flexDirection)
    | `justifyContent(justify)
    | `alignContent(align)
    | `alignItems(align)
    | `alignSelf(align)
    | `width(scalar)
    | `border(Border.t)
    | `height(scalar)
    | `padding(scalar, scalar, scalar, scalar)
    | `margin(scalar, scalar, scalar, scalar)
  ];

  let isUndefined = Encoding.isUndefined;

  let int_of_scalar = s => s |> Encoding.scalarToFloat |> int_of_float;

  let applyCommonStyle = (style, attr: [> style]) =>
    switch (attr) {
    | `position(({position, left, top, right, bottom}: Position.t)) =>
      style.positionType = (
        switch (position) {
        | `absolute => Absolute
        | `relative => Relative
        }
      );
      if (!isUndefined(left)) {
        style.left = int_of_scalar(left);
      };
      if (!isUndefined(top)) {
        style.top = int_of_scalar(top);
      };
      if (!isUndefined(right)) {
        style.right = int_of_scalar(right);
      };
      if (!isUndefined(bottom)) {
        style.bottom = int_of_scalar(bottom);
      };
    | `flexDirection(d) => style.flexDirection = d
    | `justifyContent(j) => style.justifyContent = j
    | `alignContent(a) => style.alignContent = a
    | `alignItems(a) => style.alignItems = a
    | `alignSelf(a) => style.alignItems = a
    | `width(w) => style.width = int_of_scalar(w)
    | `height(h) => style.height = int_of_scalar(h)
    | `border(({width, _}: Border.t)) =>
      if (!isUndefined(width)) {
        style.border = int_of_scalar(width);
      }
    | `padding(l, t, r, b) =>
      if (!isUndefined(l)) {
        style.paddingLeft = int_of_scalar(l);
      };
      if (!isUndefined(t)) {
        style.paddingTop = int_of_scalar(t);
      };
      if (!isUndefined(r)) {
        style.paddingRight = int_of_scalar(r);
      };
      if (!isUndefined(b)) {
        style.paddingBottom = int_of_scalar(b);
      };
    | `margin(l, t, r, b) =>
      if (!isUndefined(l)) {
        style.marginLeft = int_of_scalar(l);
      };
      if (!isUndefined(t)) {
        style.marginTop = int_of_scalar(t);
      };
      if (!isUndefined(r)) {
        style.marginRight = int_of_scalar(r);
      };
      if (!isUndefined(b)) {
        style.marginBottom = int_of_scalar(b);
      };
    | _ => ()
    };

  let makeLayoutNode = (~style, hostView: Node.context) => {
    let andStyle = LayoutSupport.defaultStyle;

    List.iter(applyCommonStyle(andStyle), style);
    LayoutSupport.createNode(~withChildren=[||], ~andStyle, hostView);
  };

  let cssNodeInsertChild = (node, child, index) => {
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

    markDirtyInternal(node);
  };
};
