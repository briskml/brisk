module Create = (Node: Flex.Spec.Node, Encoding: Flex.Spec.Encoding) => {
  include Flex.Layout.Create(Node, Encoding);
  include Style.Create(Encoding);

  module Color = Color0;

  open Encoding;
  open LayoutSupport;
  open LayoutTypes;

  let flexDirection = d =>
    `FlexDirection(
      switch (d) {
      | `Column => Column
      | `ColumnReverse => ColumnReverse
      | `Row => Row
      | `RowReverse => RowReverse
      },
    );

  let convertAlign =
    fun
    | `Auto => AlignAuto
    | `FlexStart => AlignFlexStart
    | `Center => AlignCenter
    | `FlexEnd => AlignFlexEnd
    | `Stretch => AlignStretch;

  let justifyContent = j =>
    `JustifyContent(
      switch (j) {
      | `FlexStart => JustifyFlexStart
      | `Center => JustifyCenter
      | `FlexEnd => JustifyFlexEnd
      | `SpaceBetween => JustifySpaceBetween
      | `SpaceAround => JustifySpaceAround
      },
    );

  let alignContent = a => `AlignContent(convertAlign(a));
  let alignItems = a => `AlignItems(convertAlign(a));
  let alignSelf = a => `AlignSelf(convertAlign(a));

  type style = [
    | `Position(Position.t)
    | `FlexDirection(flexDirection)
    | `JustifyContent(justify)
    | `AlignContent(align)
    | `AlignItems(align)
    | `AlignSelf(align)
    | `Width(scalar)
    | `Height(scalar)
    | `Border(Border.t)
    | `Padding(inset)
    | `Margin(inset)
  ];

  let isUndefined = Encoding.isUndefined;

  let int_of_scalar = s => s |> Encoding.scalarToFloat |> int_of_float;

  let applyCommonStyle = (style: cssStyle, attr: [> style]) =>
    switch (attr) {
    | `Position(({position, inset}: Position.t)) =>
      let positionType =
        switch (position) {
        | `Absolute => Absolute
        | `Relative => Relative
        };
      let {left, top, right, bottom}: inset = inset;

      {
        ...style,
        positionType,
        left: !isUndefined(left) ? int_of_scalar(left) : style.left,
        top: !isUndefined(top) ? int_of_scalar(top) : style.top,
        right: !isUndefined(right) ? int_of_scalar(right) : style.right,
        bottom: !isUndefined(bottom) ? int_of_scalar(bottom) : style.bottom,
      };
    | `FlexDirection(flexDirection) => {...style, flexDirection}
    | `JustifyContent(justifyContent) => {...style, justifyContent}
    | `AlignContent(alignContent) => {...style, alignContent}
    | `AlignItems(alignItems) => {...style, alignItems}
    | `AlignSelf(alignSelf) => {...style, alignSelf}
    | `Width(w) => {...style, width: int_of_scalar(w)}
    | `Height(h) => {...style, height: int_of_scalar(h)}
    | `Border(({width, _}: Border.t)) =>
      {
        ...style,
        border: !isUndefined(width) ? int_of_scalar(width) : style.border,
      }
    | `Padding({left, top, right, bottom}) => {
        ...style,
        paddingLeft:
          !isUndefined(left) ? int_of_scalar(left) : style.paddingLeft,
        paddingTop:
          !isUndefined(top) ? int_of_scalar(top) : style.paddingTop,
        paddingRight:
          !isUndefined(right) ? int_of_scalar(right) : style.paddingRight,
        paddingBottom:
          !isUndefined(bottom) ? int_of_scalar(bottom) : style.paddingBottom,
      }
    | `Margin({left, top, right, bottom}) => {
        ...style,
        marginLeft:
          !isUndefined(left) ? int_of_scalar(left) : style.marginLeft,
        marginTop: !isUndefined(top) ? int_of_scalar(top) : style.marginTop,
        marginRight:
          !isUndefined(right) ? int_of_scalar(right) : style.marginRight,
        marginBottom:
          !isUndefined(bottom) ? int_of_scalar(bottom) : style.marginBottom,
      }
    | _ => style
    };

  let makeLayoutNode = (~measure=?, ~style=[], hostView: Node.context) => {
    let accum = {
      ...LayoutSupport.defaultStyle,
      direction: LayoutSupport.defaultStyle.direction,
    };

    let andStyle = List.fold_left(applyCommonStyle, accum, style);
    LayoutSupport.createNode(
      ~withChildren=[||],
      ~andStyle,
      ~andMeasure=?measure,
      hostView,
    );
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
