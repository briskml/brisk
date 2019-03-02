module Create = (Node: Flex.Spec.Node, Encoding: Flex.Spec.Encoding) => {
  module FlexLayout = Flex.Layout.Create(Node, Encoding);

  module Color = Color0;

  open Encoding;

  type inset = {
    left: scalar,
    top: scalar,
    right: scalar,
    bottom: scalar,
  };

  module Position = {
    type position = [ | `Absolute | `Relative];

    type t = {
      position,
      inset,
    };

    let make =
        (
          ~left=cssUndefined,
          ~top=cssUndefined,
          ~right=cssUndefined,
          ~bottom=cssUndefined,
          position,
        ) => {
      let pos: t = {
        position,
        inset: {
          left,
          top,
          right,
          bottom,
        },
      };
      `Position(pos);
    };
  };

  module Font = {
    type weight = [
      | `UltraLight
      | `Thin
      | `Light
      | `Regular
      | `Medium
      | `Semibold
      | `Bold
      | `Heavy
      | `Black
    ];

    type t = {
      family: string,
      size: scalar,
      weight,
    };

    let make = (~family="", ~size=cssUndefined, ~weight=`Regular, ()) => {
      let font: t = {family, size, weight};
      `Font(font);
    };
  };

  module Alignment = {
    type t = [ | `Left | `Right | `Center | `Justified | `Natural];

    let make = (alignment: t) => `Align(alignment);
  };

  module LineBreak = {
    type t = [
      | `WordWrap
      | `CharWrap
      | `Clip
      | `TruncateHead
      | `TruncateTale
      | `TruncateMiddle
    ];

    let make = (mode: t) => `LineBreak(mode);
  };

  module Border = {
    type t = {
      width: scalar,
      radius: scalar,
      color: Color0.t,
    };

    let make =
        (
          ~width=cssUndefined,
          ~radius=cssUndefined,
          ~color=Color0.undefined,
          (),
        ) => {
      let border: t = {width, radius, color};
      `Border(border);
    };

    let width = (width: Encoding.scalar) => make(~width, ());
    let color = (color: Color0.t) => make(~color, ());
  };

  module Shadow = {
    type t = {
      x: scalar,
      y: scalar,
      opacity: scalar,
      blur: scalar,
      color: Color0.t,
    };

    let make =
        (
          ~x=cssUndefined,
          ~y=cssUndefined,
          ~opacity=cssUndefined,
          ~blur=cssUndefined,
          ~color=Color0.undefined,
          (),
        ) => {
      let shadow: t = {x, y, opacity, blur, color};
      `Shadow(shadow);
    };
  };

  let position = Position.make;
  let border = Border.make;
  let shadow = Shadow.make;
  let font = Font.make;
  let kern = f => `Kern(f);
  let align = Alignment.make;
  let lineBreak = LineBreak.make;
  let lineSpacing = f => `LineSpacing(f);

  let width = (w: scalar) => `Width(w);
  let height = (h: scalar) => `Height(h);

  let color = (color: Color0.t) => `Color(color);
  let background = (color: Color0.t) => `Background(color);

  let padding = (p: scalar) =>
    `Padding({left: p, top: p, right: p, bottom: p});
  let padding2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `Padding({left: h, top: v, right: h, bottom: v});
  let padding4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `Padding({left, top, right, bottom});

  let margin = (m: scalar) =>
    `Margin({left: m, top: m, right: m, bottom: m});

  let margin2 = (~h=cssUndefined, ~v=cssUndefined, ()) =>
    `Margin({left: h, top: v, right: h, bottom: v});
  let margin4 =
      (
        ~left=cssUndefined,
        ~top=cssUndefined,
        ~right=cssUndefined,
        ~bottom=cssUndefined,
        (),
      ) =>
    `Margin({left, top, right, bottom});

  open FlexLayout.LayoutSupport;
  open LayoutTypes;

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
  let flexBasis = s => `FlexBasis(s);

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

  module Node = {
    type flexNode = node;
    type t = {
      container: flexNode,
      content: flexNode,
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

    let removeChild = (node, child) => {
      node.children =
        Array.to_seq(node.children)
        |> Seq.filter(p => p === child)
        |> Array.of_seq;

      node.childrenCount = node.childrenCount - 1;

      markDirtyInternal(node);
    };
  };
};
