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
    | `padding(inset)
    | `margin(inset)
  ];

  let isUndefined = Encoding.isUndefined;

  let int_of_scalar = s => s |> Encoding.scalarToFloat |> int_of_float;

  let applyCommonStyle = (style: cssStyle, attr: [> style]) =>
    switch (attr) {
    | `position(({position, inset}: Position.t)) =>
      let positionType =
        switch (position) {
        | `absolute => Absolute
        | `relative => Relative
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
    | `flexDirection(flexDirection) => {...style, flexDirection}
    | `justifyContent(justifyContent) => {...style, justifyContent}
    | `alignContent(alignContent) => {...style, alignContent}
    | `alignItems(alignItems) => {...style, alignItems}
    | `alignSelf(alignSelf) => {...style, alignSelf}
    | `width(w) => {...style, width: int_of_scalar(w)}
    | `height(h) => {...style, height: int_of_scalar(h)}
    | `border(({width, _}: Border.t)) => {
        ...style,
        border: !isUndefined(width) ? int_of_scalar(width) : style.width,
      }
    | `padding({left, top, right, bottom}) => {
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
    | `margin({left, top, right, bottom}) => {
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
