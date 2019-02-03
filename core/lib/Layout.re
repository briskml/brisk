module Create = (Node: Flex.Spec.Node, Encoding: Flex.Spec.Encoding) => {
  module FlexLayout = Flex.Layout.Create(Node, Encoding);
  module Attributes = StyleAttributes.Create(Encoding);

  module Color = Color0;

  open Encoding;
  open FlexLayout.LayoutSupport;
  open LayoutTypes;
  open Attributes;

  let flex = s => `Flex(s);

  let flexDirection = d =>
    `FlexDirection(
      switch (d) {
      | `Column => Column
      | `ColumnReverse => ColumnReverse
      | `Row => Row
      | `RowReverse => RowReverse
      },
    );

  let flexGrow = s => `FlexGrow(s);
  let flexShrink = s => `FlexShrink(s);
  let flexBasis = s => `Flex(s);

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

  let overflow = o =>
    `Overflow(
      switch (o) {
      | `Visible => Visible
      | `Hidden => Hidden
      | `Scroll => Scroll
      },
    );

  let alignContent = a => `AlignContent(convertAlign(a));
  let alignItems = a => `AlignItems(convertAlign(a));
  let alignSelf = a => `AlignSelf(convertAlign(a));

  type style = [
    | `Position(Position.t)
    | `Flex(scalar)
    | `FlexDirection(flexDirection)
    | `FlexGrow(scalar)
    | `FlexShrink(scalar)
    | `FlexBasis(scalar)
    | `JustifyContent(justify)
    | `AlignContent(align)
    | `AlignItems(align)
    | `AlignSelf(align)
    | `Overflow(overflow)
    | `Width(scalar)
    | `Height(scalar)
    | `Border(Border.t)
    | `Padding(inset)
    | `Margin(inset)
  ];

  let isUndefined = Encoding.isUndefined;

  let int_of_scalar = s => s |> Encoding.scalarToFloat |> int_of_float;

  let applyCommonStyle = (style: cssStyle, attribute: [> style]) =>
    switch (attribute) {
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
    | `Flex(f) => {...style, flex: int_of_scalar(f)}
    | `FlexDirection(flexDirection) => {...style, flexDirection}
    | `FlexGrow(f) => {...style, flexGrow: int_of_scalar(f)}
    | `FlexShrink(f) => {...style, flexShrink: int_of_scalar(f)}
    | `FlexBasis(f) => {...style, flexBasis: int_of_scalar(f)}
    | `JustifyContent(justifyContent) => {...style, justifyContent}
    | `AlignContent(alignContent) => {...style, alignContent}
    | `AlignItems(alignItems) => {...style, alignItems}
    | `AlignSelf(alignSelf) => {...style, alignSelf}
    | `Width(w) => {...style, width: int_of_scalar(w)}
    | `Height(h) => {...style, height: int_of_scalar(h)}
    | `Overflow(overflow) => {...style, overflow}
    | `Border(({width, _}: Border.t)) => {
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

  module LayoutNode = {
    type flexNode = node;
    type t = {
      container: node,
      content: node,
    };

    module Composite = {
      let makeFlexNode = (~measure=?, ~style=[], hostView: Node.context) => {
        let accum = {
          ...FlexLayout.LayoutSupport.defaultStyle,
          direction: FlexLayout.LayoutSupport.defaultStyle.direction,
        };

        let andStyle = List.fold_left(applyCommonStyle, accum, style);
        FlexLayout.LayoutSupport.createNode(
          ~withChildren=[||],
          ~andStyle,
          ~andMeasure=?measure,
          hostView,
        );
      };

      let make = (~container, ~content) => {container, content};
    };

    let make = (~measure=?, ~style=[], hostView: Node.context) => {
      let node = Composite.makeFlexNode(~measure?, ~style, hostView);
      {container: node, content: node};
    };

    let insertChild = (node, child, index) => {
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
};
