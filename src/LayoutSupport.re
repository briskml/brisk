let nodeWithNoMeasureStr = "Passed node with no measurement function";

module Create = (Node: Spec.Node, Encoding: Spec.Encoding) => {

  /***
   * Copyright (c) 2014, Facebook, Inc.
   * All rights reserved.
   *
   * This source code is licensed under the BSD-style license found in the
   * LICENSE file in the root directory of this source tree. An additional grant
   * of patent rights can be found in the PATENTS file in the same directory.
   * Converted to Reason:
   * Replace:
   *
   *   function() {
   *
   * With:
   *   fun () => {
   *
   * :%s/\<var\>/let/g
   *
   * For enums:
   * Either:
   *
   *    :%s/\<\([A-Z_]\+\)\>/\L&/g
   *
   * Or Better:
   *   Just define the types as such below, and then regex away the constants
   *   value declarations.
   *
   * %s/\_s*let \<\([A-Z_]\+\)\> = .*;//g
   *
   * Since we've defined our own css undefined, use it.
   * :%s/\<CSS_UNDEFINED\>/CSS_UNDEFINED/g
   */

  /***
   * So we can replace:
   * `!blah(x,y)` with `not <| blah(x,y)` with the same precedence.
   */
  module LayoutTypes = LayoutTypes.Create(Node, Encoding);
  open LayoutTypes;
  open HardCodedEncoding;
  let positive_flex_is_auto = false;
  let gCurrentGenerationCount = 0;
  let failOnDummyMeasure = true;
  /* static const YGEdge leading[4] = { */
  /*         [YGFlexDirectionColumn] = YGEdgeTop, */
  /*         [YGFlexDirectionColumnReverse] = YGEdgeBottom, */
  /*         [YGFlexDirectionRow] = YGEdgeLeft, */
  /*         [YGFlexDirectionRowReverse] = YGEdgeRight, */
  /* }; */
  let fminf = (a, b) =>
    if (isUndefined(b) || a < b) {
      a
    } else {
      b
    };
  let fmaxf = (a, b) =>
    if (isUndefined(b) || a > b) {
      a
    } else {
      b
    };
  let cssGetFlexGrow = (node) =>
    if (! isUndefined(node.style.flexGrow)) {
      node.style.flexGrow
    } else if (! isUndefined(node.style.flex) && node.style.flex > zero) {
      node.style.flex
    } else {
      zero
    };
  let cssGetFlexShrink = (node) =>
    if (! isUndefined(node.style.flexShrink)) {
      node.style.flexShrink
    } else if (! isUndefined(node.style.flex) && node.style.flex < zero) {
      -. node.style.flex
    } else {
      zero
    };
  let cssGetFlexBasis = (node) =>
    if (! isUndefined(node.style.flexBasis)) {
      node.style.flexBasis
    } else if (! isUndefined(node.style.flex)) {
      if (node.style.flex > zero) {
        zero
      } else {
        cssUndefined
      }
    } else {
      cssUndefined
    };

  /***
   * Computes the leading *concrete* edge (not Start/End etc).
   */
  let leadingEdgeForAxis = (axis) =>
    switch axis {
    | Column => Top
    | ColumnReverse => Bottom
    | Row => Left
    | RowReverse => Right
    };
  /* static const YGEdge trailing[4] = { */
  /*         [YGFlexDirectionColumn] = YGEdgeBottom, */
  /*         [YGFlexDirectionColumnReverse] = YGEdgeTop, */
  /*         [YGFlexDirectionRow] = YGEdgeRight, */
  /*         [YGFlexDirectionRowReverse] = YGEdgeLeft, */
  /* }; */
  let trailingEdgeForAxis = (axis) =>
    switch axis {
    | Column => Bottom
    | ColumnReverse => Top
    | Row => Right
    | RowReverse => Left
    };
  let debugLog = (txt) => Printf.printf("%s\n%!", txt);
  /* static const YGEdge pos[4] = { */
  /*         [YGFlexDirectionColumn] = YGEdgeTop, */
  /*         [YGFlexDirectionColumnReverse] = YGEdgeBottom, */
  /*         [YGFlexDirectionRow] = YGEdgeLeft, */
  /*         [YGFlexDirectionRowReverse] = YGEdgeRight, */
  /* }; */
  /* static const YGDimension dim[4] = { */
  /*         [YGFlexDirectionColumn] = YGDimensionHeight, */
  /*         [YGFlexDirectionColumnReverse] = YGDimensionHeight, */
  /*         [YGFlexDirectionRow] = YGDimensionWidth, */
  /*         [YGFlexDirectionRowReverse] = YGDimensionWidth, */
  /* }; */
  let positionForEdge = (style, edge) =>
    switch edge {
    | Start => style.start
    | End => style.endd
    | Left => style.left
    | Right => style.right
    | Vertical => style.vertical
    | Horizontal => style.horizontal
    | Top => style.top
    | Bottom => style.bottom
    | All => style.position
    };
  let marginForEdge = (style, edge) =>
    switch edge {
    | Start => style.marginStart
    | End => style.marginEnd
    | Left => style.marginLeft
    | Right => style.marginRight
    | Vertical => style.marginVertical
    | Horizontal => style.marginHorizontal
    | Top => style.marginTop
    | Bottom => style.marginBottom
    | All => style.margin
    };
  let borderForEdge = (style, edge) =>
    switch edge {
    | Start => style.borderStart
    | End => style.borderEnd
    | Left => style.borderLeft
    | Right => style.borderRight
    | Vertical => style.borderVertical
    | Horizontal => style.borderHorizontal
    | Top => style.borderTop
    | Bottom => style.borderBottom
    | All => style.border
    };
  let paddingForEdge = (style, edge) =>
    switch edge {
    | Start => style.paddingStart
    | End => style.paddingEnd
    | Left => style.paddingLeft
    | Right => style.paddingRight
    | Vertical => style.paddingVertical
    | Horizontal => style.paddingHorizontal
    | Top => style.paddingTop
    | Bottom => style.paddingBottom
    | All => style.padding
    };
  let computedEdgeValuePositionOriginal = (style, edge, defaultValue) =>
    if (! isUndefined(positionForEdge(style, edge))) {
      positionForEdge(style, edge)
    } else if ((edge === Top || edge === Bottom) && ! isUndefined(style.vertical)) {
      style.vertical
    } else if ((edge === Left || edge === Right || edge === Start || edge === End)
               && ! isUndefined(style.horizontal)) {
      style.horizontal
    } else if (! isUndefined(style.position)) {
      style.position
    } else if (edge === Start || edge === End) {
      cssUndefined
    } else {
      defaultValue
    };

  /***
   * This should always called with a concrete edge, not Horizontal etc. We
   * could actually make a new datatype `concreteEdge` to model this.
   * Polymorphic variants would also solve it nicely.
   * computedEdgeValuePosition is the version of computedEdgeValuePosition_ but
   * with calls to `positionForEdge` inlined.
   */
  let computedEdgeValuePosition = (style, edge, defaultValue) =>
    /* Consider passing the default (positionForEdge edge) */
    /* wtf: Factoring out positionForEdge causes ten pct slowdown. */
    switch edge {
    | Start =>
      if (! isUndefined(style.start)) {
        style.start
      } else if (! isUndefined(style.horizontal)) {
        style.horizontal
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        cssUndefined
      }
    | End =>
      if (! isUndefined(style.endd)) {
        style.endd
      } else if (! isUndefined(style.horizontal)) {
        style.horizontal
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        cssUndefined
      }
    | Left =>
      if (! isUndefined(style.left)) {
        style.left
      } else if (! isUndefined(style.horizontal)) {
        style.horizontal
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        defaultValue
      }
    | Right =>
      if (! isUndefined(style.right)) {
        style.right
      } else if (! isUndefined(style.horizontal)) {
        style.horizontal
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        defaultValue
      }
    | Top =>
      if (! isUndefined(style.top)) {
        style.top
      } else if (! isUndefined(style.vertical)) {
        style.vertical
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        defaultValue
      }
    | Bottom =>
      if (! isUndefined(style.bottom)) {
        style.bottom
      } else if (! isUndefined(style.vertical)) {
        style.vertical
      } else if (! isUndefined(style.position)) {
        style.position
      } else {
        defaultValue
      }
    /* | Vertical => */
    /*   not (isUndefined style.vertical) ? */
    /*     style.vertical : not (isUndefined style.position) ? style.position : defaultValue */
    /* | Horizontal => */
    /*   not (isUndefined style.horizontal) ? */
    /*     style.horizontal : not (isUndefined style.position) ? style.position : defaultValue */
    /* | All => */
    /*   not (isUndefined style.position) ? */
    /*     style.position : not (isUndefined style.position) ? style.position : defaultValue */
    | _ => raise(Invalid_argument("Should never be called with a non-concrete edge"))
    };
  let computedEdgeValueMarginOriginal = (style, edge, defaultValue) => {
    /* Consider passing the default (marginForEdge edge) */
    let forEdge = marginForEdge(style, edge);
    if (! isUndefined(forEdge)) {
      forEdge
    } else if ((edge === Top || edge === Bottom) && ! isUndefined(style.marginVertical)) {
      style.marginVertical
    } else if ((edge === Left || edge === Right || edge === Start || edge === End)
               && ! isUndefined(style.marginHorizontal)) {
      style.marginHorizontal
    } else {
      let margin = style.margin;
      if (! isUndefined(margin)) {
        style.margin
      } else if (edge === Start || edge === End) {
        cssUndefined
      } else {
        defaultValue
      }
    }
  };
  let computedEdgeValueMargin = (style, edge, defaultValue) =>
    switch edge {
    | Start =>
      if (! isUndefined(style.marginStart)) {
        style.marginStart
      } else if (! isUndefined(style.marginHorizontal)) {
        style.marginHorizontal
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        cssUndefined
      }
    | End =>
      if (! isUndefined(style.marginEnd)) {
        style.marginEnd
      } else if (! isUndefined(style.marginHorizontal)) {
        style.marginHorizontal
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        cssUndefined
      }
    | Left =>
      if (! isUndefined(style.marginLeft)) {
        style.marginLeft
      } else if (! isUndefined(style.marginHorizontal)) {
        style.marginHorizontal
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        defaultValue
      }
    | Right =>
      if (! isUndefined(style.marginRight)) {
        style.marginRight
      } else if (! isUndefined(style.marginHorizontal)) {
        style.marginHorizontal
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        defaultValue
      }
    | Top =>
      if (! isUndefined(style.marginTop)) {
        style.marginTop
      } else if (! isUndefined(style.marginVertical)) {
        style.marginVertical
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        defaultValue
      }
    | Bottom =>
      if (! isUndefined(style.marginBottom)) {
        style.marginBottom
      } else if (! isUndefined(style.marginVertical)) {
        style.marginVertical
      } else if (! isUndefined(style.margin)) {
        style.margin
      } else {
        defaultValue
      }
    /* | Vertical => */
    /*   not (isUndefined style.marginVertical) ? */
    /*     style.marginVertical : not (isUndefined style.margin) ? style.margin : defaultValue */
    /* | Horizontal => */
    /*   not (isUndefined style.marginHorizontal) ? */
    /*     style.marginHorizontal : not (isUndefined style.margin) ? style.margin : defaultValue */
    /* | All => */
    /*   not (isUndefined style.margin) ? */
    /*     style.margin : not (isUndefined style.margin) ? style.margin : defaultValue */
    | _ => raise(Invalid_argument("Should never be called with a non-concrete edge"))
    };
  let computedEdgeValueBorderOriginal = (style, edge, defaultValue) => {
    /* Consider passing the default (borderForEdge edge) */
    let forEdge = borderForEdge(style, edge);
    if (! isUndefined(forEdge)) {
      forEdge
    } else if ((edge === Top || edge === Bottom) && ! isUndefined(style.borderVertical)) {
      style.borderVertical
    } else if ((edge === Left || edge === Right || edge === Start || edge === End)
               && ! isUndefined(style.borderHorizontal)) {
      style.borderHorizontal
    } else {
      let border = style.border;
      if (! isUndefined(border)) {
        style.border
      } else if (edge === Start || edge === End) {
        cssUndefined
      } else {
        defaultValue
      }
    }
  };
  let computedEdgeValueBorder = (style, edge, defaultValue) =>
    switch edge {
    | Start =>
      if (! isUndefined(style.borderStart)) {
        style.borderStart
      } else if (! isUndefined(style.borderHorizontal)) {
        style.borderHorizontal
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        cssUndefined
      }
    | End =>
      if (! isUndefined(style.borderEnd)) {
        style.borderEnd
      } else if (! isUndefined(style.borderHorizontal)) {
        style.borderHorizontal
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        cssUndefined
      }
    | Left =>
      if (! isUndefined(style.borderLeft)) {
        style.borderLeft
      } else if (! isUndefined(style.borderHorizontal)) {
        style.borderHorizontal
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        defaultValue
      }
    | Right =>
      if (! isUndefined(style.borderRight)) {
        style.borderRight
      } else if (! isUndefined(style.borderHorizontal)) {
        style.borderHorizontal
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        defaultValue
      }
    | Top =>
      if (! isUndefined(style.borderTop)) {
        style.borderTop
      } else if (! isUndefined(style.borderVertical)) {
        style.borderVertical
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        defaultValue
      }
    | Bottom =>
      if (! isUndefined(style.borderBottom)) {
        style.borderBottom
      } else if (! isUndefined(style.borderVertical)) {
        style.borderVertical
      } else if (! isUndefined(style.border)) {
        style.border
      } else {
        defaultValue
      }
    /* | Vertical => */
    /*   not (isUndefined style.borderVertical) ? */
    /*     style.borderVertical : not (isUndefined style.border) ? style.border : defaultValue */
    /* | Horizontal => */
    /*   not (isUndefined style.borderHorizontal) ? */
    /*     style.borderHorizontal : not (isUndefined style.border) ? style.border : defaultValue */
    /* | All => */
    /*   not (isUndefined style.border) ? */
    /*     style.border : not (isUndefined style.border) ? style.border : defaultValue */
    | _ => raise(Invalid_argument("Should never be called with a non-concrete edge"))
    };
  let computedEdgeValuePaddingOriginal = (style, edge, defaultValue) => {
    let forEdge = paddingForEdge(style, edge);
    if (! isUndefined(forEdge)) {
      forEdge
    } else if ((edge === Top || edge === Bottom) && ! isUndefined(style.paddingVertical)) {
      style.paddingVertical
    } else if ((edge === Left || edge === Right || edge === Start || edge === End)
               && ! isUndefined(style.paddingHorizontal)) {
      style.paddingHorizontal
    } else {
      let padding = style.padding;
      if (! isUndefined(padding)) {
        style.padding
      } else if (edge === Start || edge === End) {
        cssUndefined
      } else {
        defaultValue
      }
    }
  };
  let computedEdgeValuePadding = (style, edge, defaultValue) =>
    switch edge {
    | Start =>
      if (! isUndefined(style.paddingStart)) {
        style.paddingStart
      } else if (! isUndefined(style.paddingHorizontal)) {
        style.paddingHorizontal
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        cssUndefined
      }
    | End =>
      if (! isUndefined(style.paddingEnd)) {
        style.paddingEnd
      } else if (! isUndefined(style.paddingHorizontal)) {
        style.paddingHorizontal
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        cssUndefined
      }
    | Left =>
      if (! isUndefined(style.paddingLeft)) {
        style.paddingLeft
      } else if (! isUndefined(style.paddingHorizontal)) {
        style.paddingHorizontal
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        defaultValue
      }
    | Right =>
      if (! isUndefined(style.paddingRight)) {
        style.paddingRight
      } else if (! isUndefined(style.paddingHorizontal)) {
        style.paddingHorizontal
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        defaultValue
      }
    | Top =>
      if (! isUndefined(style.paddingTop)) {
        style.paddingTop
      } else if (! isUndefined(style.paddingVertical)) {
        style.paddingVertical
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        defaultValue
      }
    | Bottom =>
      if (! isUndefined(style.paddingBottom)) {
        style.paddingBottom
      } else if (! isUndefined(style.paddingVertical)) {
        style.paddingVertical
      } else if (! isUndefined(style.padding)) {
        style.padding
      } else {
        defaultValue
      }
    /* | Vertical => */
    /*   not (isUndefined style.paddingVertical) ? */
    /*     style.paddingVertical : not (isUndefined style.padding) ? style.padding : defaultValue */
    /* | Horizontal => */
    /*   not (isUndefined style.paddingHorizontal) ? */
    /*     style.paddingHorizontal : not (isUndefined style.padding) ? style.padding : defaultValue */
    /* | All => */
    /*   not (isUndefined style.padding) ? */
    /*     style.padding : not (isUndefined style.padding) ? style.padding : defaultValue */
    | _ => raise(Invalid_argument("Should never be called with a non-concrete edge"))
    };

  /***
   * In the integer encoding of all
   */
  let dummyMeasure = (context, width, widthMeasureMode, height, heightMeasureMode) =>
    if (failOnDummyMeasure) {
      raise(
        Invalid_argument(
          "A node does not have its measure function " ++ " implemented, yet requires measurement"
        )
      )
    } else {
      {LayoutTypes.width: zero, height: zero}
    };
  /*
      node->style.align_items = AlignStretch;
      node->style.align_content = AlignFlexStart;

      node->style.direction = Inherit;
      node->style.flex_direction = Column;

      node->style.overflow = Visible;


      node->layout.dimensions[CSS_WIDTH] = CSS_UNDEFINED;
      node->layout.dimensions[CSS_HEIGHT] = CSS_UNDEFINED;

   // Such that the comparison is always going to be false
   node->layout.last_parent_direction = (css_direction_t)-1;
   node->layout.should_update = true;
   node->layout.next_cached_measurements_index = 0;

   node->layout.measured_dimensions[CSS_WIDTH] = CSS_UNDEFINED;
   node->layout.measured_dimensions[CSS_HEIGHT] = CSS_UNDEFINED;
   node->layout.cached_layout.width_measure_mode = (css_measure_mode_t)-1;
   node->layout.cached_layout.height_measure_mode = (css_measure_mode_t)-1;

    */
  let dummyCachedMeasurement = {
    availableWidth: zero,
    availableHeight: zero,
    widthMeasureMode: CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS,
    heightMeasureMode: CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS,
    /* TODO: Fix / check this (should be negative one perhaps?) */
    computedWidth: negativeOne,
    computedHeight: negativeOne
  };
  let defaultStyle = {
    direction: Inherit,
    flexDirection: Column,
    justifyContent: JustifyFlexStart,
    alignContent: AlignFlexStart,
    alignItems: AlignStretch,
    alignSelf: AlignAuto,
    positionType: Relative,
    flexWrap: CssNoWrap,
    overflow: Visible,
    /* TODO: What is this flex property and why am I paying for it! */
    flex: cssUndefined,
    /* TODO: Fix / check this. */
    flexGrow: cssUndefined,
    flexShrink: cssUndefined,
    /* TODO: Fix / check this */
    flexBasis: cssUndefined,
    marginLeft: cssUndefined,
    marginTop: cssUndefined,
    marginRight: cssUndefined,
    marginBottom: cssUndefined,
    paddingLeft: cssUndefined,
    paddingTop: cssUndefined,
    paddingRight: cssUndefined,
    paddingBottom: cssUndefined,
    borderLeft: cssUndefined,
    borderTop: cssUndefined,
    borderRight: cssUndefined,
    borderBottom: cssUndefined,
    width: cssUndefined,
    height: cssUndefined,
    /* TODO: Fix / check this. (https://github.com/facebook/css-layout) */
    minWidth: cssUndefined,
    minHeight: cssUndefined,
    maxWidth: cssUndefined,
    maxHeight: cssUndefined,
    left: cssUndefined,
    top: cssUndefined,
    right: cssUndefined,
    bottom: cssUndefined,
    start: cssUndefined,
    endd: cssUndefined,
    marginStart: cssUndefined,
    marginEnd: cssUndefined,
    paddingStart: cssUndefined,
    paddingEnd: cssUndefined,
    borderStart: cssUndefined,
    borderEnd: cssUndefined,
    /***
     * All of these need to be reevaluated (to see if we really want them at
     * zero or cssUndefined).
     */
    horizontal: cssUndefined,
    vertical: cssUndefined,
    position: cssUndefined,
    padding: cssUndefined,
    paddingHorizontal: cssUndefined,
    paddingVertical: cssUndefined,
    margin: cssUndefined,
    marginVertical: cssUndefined,
    marginHorizontal: cssUndefined,
    borderHorizontal: cssUndefined,
    borderVertical: cssUndefined,
    border: cssUndefined
  };
  /* Create a copy of cachedMeasurement */
  let createCacheMeasurement = () => {...dummyCachedMeasurement, availableWidth: zero};
  let rec theNullNode = {
    selfRef: Nativeint.zero,
    children: [||],
    childrenCount: 0,
    style: defaultStyle,
    hasNewLayout: true,
    layout: {
      direction: Inherit,
      /* Instead of recomputing the entire layout every single time, we
       * cache some information to break early when nothing changed */
      generationCount: 0,
      computedFlexBasisGeneration: 0,
      lastParentDirection: CSS_DIRECTION_NEGATIVE_ONE_WHATEVER_THAT_MEANS,
      nextCachedMeasurementsIndex: 0,
      /***
       * Hardcoded to 6 previous measurements.
       */
      cachedMeasurement1: dummyCachedMeasurement,
      cachedMeasurement2: dummyCachedMeasurement,
      cachedMeasurement3: dummyCachedMeasurement,
      cachedMeasurement4: dummyCachedMeasurement,
      cachedMeasurement5: dummyCachedMeasurement,
      cachedMeasurement6: dummyCachedMeasurement,
      cachedLayout: dummyCachedMeasurement,
      /***
       * Start out as zero.
       */
      computedFlexBasis: cssUndefined,
      left: zero,
      top: zero,
      right: zero,
      bottom: zero,
      /***
       * Start out as undefined.
       */
      width: cssUndefined,
      height: cssUndefined,
      measuredWidth: cssUndefined,
      measuredHeight: cssUndefined
    },
    lineIndex: 0,
    /***
     * As a clever trick, to encode "NULL" node, we can create a recursive
     * binding and point nextChild to itself, and interpreting that as NULL.
     */
    parent: theNullNode,
    nextChild: theNullNode,
    measure: None,
    print: None,
    isDirty: false,
    context: Node.nullContext
  };
  /* Force allocating a new object */
  let createLayout = () => {
    ...theNullNode.layout,
    direction: Inherit,
    cachedLayout: createCacheMeasurement()
  };
  let createNode = (~withChildren as children, ~andStyle as style, ~andMeasure as m=?, context) => {
    ...theNullNode,
    children,
    childrenCount: Array.length(children),
    style,
    measure: m,
    /***
     * We can keep the original style because it's immutable, but layout is not.
     */
    layout: createLayout(),
    context
  };

  /***
   * Layout getters.
   */
  let layoutMeasuredDimensionForAxis = (node, axis) =>
    switch axis {
    | Column => node.layout.measuredHeight
    | ColumnReverse => node.layout.measuredHeight
    | Row => node.layout.measuredWidth
    | RowReverse => node.layout.measuredWidth
    };

  /***
   * Pos[] based layout position for axis.
   */
  let layoutPosPositionForAxis = (node, axis) =>
    switch axis {
    | Column => node.layout.top
    | ColumnReverse => node.layout.bottom
    | Row => node.layout.left
    | RowReverse => node.layout.right
    };

  /***
   * Leading[] based layout position for axis.
   */
  let layoutLeadingPositionForAxis = layoutPosPositionForAxis;

  /***
   * Trailing[] based layout position for axis.
   */
  let layoutTrailingPositionForAxis = (node, axis) =>
    switch axis {
    | Column => node.layout.bottom
    | ColumnReverse => node.layout.top
    | Row => node.layout.right
    | RowReverse => node.layout.left
    };

  /***
   * Dimension style getter.
   */
  let styleDimensionForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.height
    | ColumnReverse => node.style.height
    | Row => node.style.width
    | RowReverse => node.style.width
    };

  /***
   * Dimension style getter.
   */
  let styleMinDimensionForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.minHeight
    | ColumnReverse => node.style.minHeight
    | Row => node.style.minWidth
    | RowReverse => node.style.minWidth
    };

  /***
   * Leading style getters.
   */
  let styleLeadingPositionForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.top
    | ColumnReverse => node.style.bottom
    | Row => node.style.left
    | RowReverse => node.style.right
    };
  let styleLeadingPositionForAxisOrZero = (node, axis) => {
    let leadingPos = styleLeadingPositionForAxis(node, axis);
    ! isUndefined(leadingPos) ? leadingPos : zero
  };
  let styleLeadingPaddingForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.paddingTop
    | ColumnReverse => node.style.paddingBottom
    | Row => node.style.paddingLeft
    | RowReverse => node.style.paddingRight
    };
  let styleLeadingMarginForAxis = (node, leadingAxis) =>
    switch leadingAxis {
    | Column => node.style.marginTop
    | ColumnReverse => node.style.marginBottom
    | Row => node.style.marginLeft
    | RowReverse => node.style.marginRight
    };
  let styleLeadingBorderForAxis = (node, leadingAxis) =>
    switch leadingAxis {
    | Column => node.style.borderTop
    | ColumnReverse => node.style.borderBottom
    | Row => node.style.borderLeft
    | RowReverse => node.style.borderRight
    };

  /***
   * Trailing style getters.
   */
  let styleTrailingPositionForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.bottom
    | ColumnReverse => node.style.top
    | Row => node.style.right
    | RowReverse => node.style.left
    };
  let styleTrailingPositionForAxisOrZero = (node, axis) => {
    let leadingPos = styleTrailingPositionForAxis(node, axis);
    ! isUndefined(leadingPos) ? leadingPos : zero
  };
  let styleTrailingPaddingForAxis = (node, axis) =>
    switch axis {
    | Column => node.style.paddingBottom
    | ColumnReverse => node.style.paddingTop
    | Row => node.style.paddingRight
    | RowReverse => node.style.paddingLeft
    };
  let styleTrailingMarginForAxis = (node, trailingAxis) =>
    switch trailingAxis {
    | Column => node.style.marginBottom
    | ColumnReverse => node.style.marginTop
    | Row => node.style.marginRight
    | RowReverse => node.style.marginLeft
    };
  let styleTrailingBorderForAxis = (node, trailingAxis) =>
    switch trailingAxis {
    | Column => node.style.borderBottom
    | ColumnReverse => node.style.borderTop
    | Row => node.style.borderRight
    | RowReverse => node.style.borderLeft
    };
  let rec markDirtyInternal = (node) =>
    if (! node.isDirty) {
      node.isDirty = true;
      node.layout.computedFlexBasis = cssUndefined;
      if (node.parent !== theNullNode) {
        markDirtyInternal(node.parent)
      }
    };
  let markDirty = (node) => {
    /* Only leaf nodes with custom measure functions
     * should manually mark themselves as dirty */
    assert (node.measure !== None);
    markDirtyInternal(node)
  };

  /***
   * Dim[] based layout dimension setter.
   */
  let setDimLayoutDimensionForAxis = (node, axis, value) =>
    switch axis {
    | Column => node.layout.height = value
    | ColumnReverse => node.layout.height = value
    | Row => node.layout.width = value
    | RowReverse => node.layout.width = value
    };
  let setLayoutMeasuredDimensionForAxis = (node, axis, value) =>
    switch axis {
    | Column => node.layout.measuredHeight = value
    | ColumnReverse => node.layout.measuredHeight = value
    | Row => node.layout.measuredWidth = value
    | RowReverse => node.layout.measuredWidth = value
    };

  /***
   * Leading layout setter.
   */
  let setLayoutLeadingPositionForAxis = (node, axis, value) =>
    switch axis {
    | Column => node.layout.top = value
    | ColumnReverse => node.layout.bottom = value
    | Row => node.layout.left = value
    | RowReverse => node.layout.right = value
    };

  /***
   * Trailing layout setter.
   */
  let setLayoutTrailingPositionForAxis = (node, axis, value) =>
    switch axis {
    | Column => node.layout.bottom = value
    | ColumnReverse => node.layout.top = value
    | Row => node.layout.right = value
    | RowReverse => node.layout.left = value
    };

  /***
   * Leading layout setter.
   */
  let setLayoutPositionForAxis = (node, axis, leadingValue, trailingValue) =>
    switch axis {
    | Column =>
      node.layout.top = leadingValue;
      node.layout.bottom = trailingValue
    | ColumnReverse =>
      node.layout.bottom = leadingValue;
      node.layout.top = trailingValue
    | Row =>
      node.layout.left = leadingValue;
      node.layout.right = trailingValue
    | RowReverse =>
      node.layout.right = leadingValue;
      node.layout.left = trailingValue
    };
  let resolveDirection = (node, parentDirection) => {
    let direction = node.style.direction;
    if (direction === Inherit) {
      if (parentDirection !== Inherit) {
        parentDirection
      } else {
        Ltr
      }
    } else {
      direction
    }
  };
  let resolveAxis = (flex_direction, direction) =>
    if (direction === Rtl) {
      switch flex_direction {
      | Row => RowReverse
      | RowReverse => Row
      | _ => flex_direction
      }
    } else {
      flex_direction
    };
  /* let isRowDirection flexDirection => flexDirection === Row || flexDirection === RowReverse; */

  /***
   * Interestingly, if this is changed to the following, it is much slower.
   *  | Row => true
   *  | RowReverse => true
   *  | Column => false
   *  | ColumnReverse => false
   *
   * This implementation with the default _ case seems to be the fastest (it
   * may be faster than spelling out the two remaining variants).
   */
  let isRowDirection = (flexDirection) =>
    switch flexDirection {
    | Row
    | RowReverse => true
    | _ => false
    };
  /* let isColumnDirection flexDirection => flexDirection === Column || flexDirection === ColumnReverse; */
  let isColumnDirection = (flexDirection) =>
    switch flexDirection {
    | Column
    | ColumnReverse => true
    | _ => false
    };
  let getCrossFlexDirection = (flex_direction, direction) =>
    switch flex_direction {
    | Column
    | ColumnReverse =>
      /* Inlined `resolveAxis Row` here. */
      if (direction === Rtl) {
        RowReverse
      } else {
        Row
      }
    | _ => Column
    };
  let resolveAxises = (flex_direction, direction) =>
    switch (direction, flex_direction) {
    | (Rtl, Row) => (RowReverse, Column)
    | (Rtl, RowReverse) => (Row, Column)
    | (Rtl, Column) => (Column, RowReverse)
    | (Rtl, ColumnReverse) => (ColumnReverse, RowReverse)
    | (_, Column) => (Column, Row)
    | (_, ColumnReverse) => (ColumnReverse, Row)
    | (_, Row) => (Row, Column)
    | (_, RowReverse) => (RowReverse, Column)
    };
  let isFlex = (node) =>
    node.style.positionType === Relative
    && (cssGetFlexGrow(node) != zero || cssGetFlexShrink(node) != zero);
  let getLeadingMargin = (node, axis) =>
    if (isRowDirection(axis) && ! isUndefined(node.style.marginStart)) {
      node.style.marginStart
    } else {
      computedEdgeValueMargin(node.style, leadingEdgeForAxis(axis), zero)
    };
  let getTrailingMargin = (node, axis) =>
    if (isRowDirection(axis) && ! isUndefined(node.style.marginEnd)) {
      node.style.marginEnd
    } else {
      computedEdgeValueMargin(node.style, trailingEdgeForAxis(axis), zero)
    };
  let getLeadingPadding = (node, axis) =>
    if (isRowDirection(axis)
        && ! isUndefined(node.style.paddingStart)
        && node.style.paddingStart >= zero) {
      node.style.paddingStart
    } else {
      fmaxf(computedEdgeValuePadding(node.style, leadingEdgeForAxis(axis), zero), zero)
    };
  let getTrailingPadding = (node, axis) =>
    if (isRowDirection(axis)
        && ! isUndefined(node.style.paddingEnd)
        && node.style.paddingEnd >= zero) {
      node.style.paddingEnd
    } else {
      fmaxf(computedEdgeValuePadding(node.style, trailingEdgeForAxis(axis), zero), zero)
    };
  let getLeadingBorder = (node, axis) =>
    if (isRowDirection(axis)
        && ! isUndefined(node.style.borderStart)
        && node.style.borderStart >= zero) {
      node.style.borderStart
    } else {
      fmaxf(computedEdgeValueBorder(node.style, leadingEdgeForAxis(axis), zero), zero)
    };
  let getTrailingBorder = (node, axis) =>
    if (isRowDirection(axis) && ! isUndefined(node.style.borderEnd) && node.style.borderEnd >= zero) {
      node.style.borderEnd
    } else {
      fmaxf(computedEdgeValueBorder(node.style, trailingEdgeForAxis(axis), zero), zero)
    };
  let getLeadingPaddingAndBorder = (node, axis) =>
    getLeadingPadding(node, axis) +. getLeadingBorder(node, axis);
  let getTrailingPaddingAndBorder = (node, axis) =>
    getTrailingPadding(node, axis) +. getTrailingBorder(node, axis);
  let getMarginAxis = (node, axis) =>
    getLeadingMargin(node, axis) +. getTrailingMargin(node, axis);
  let getPaddingAndBorderAxis = (node, axis) =>
    getLeadingPaddingAndBorder(node, axis) +. getTrailingPaddingAndBorder(node, axis);
  let getAlignItem = (node, child) =>
    if (child.style.alignSelf === AlignAuto) {
      node.style.alignItems
    } else {
      child.style.alignSelf
    };
  let getDimWithMargin = (node, axis) =>
    layoutMeasuredDimensionForAxis(node, axis)
    +. getLeadingMargin(node, axis)
    +. getTrailingMargin(node, axis);
  let isStyleDimDefined = (node, axis) => {
    let value = styleDimensionForAxis(node, axis);
    ! isUndefined(value) && value >= zero
  };
  let isLayoutDimDefined = (node, axis) => {
    let value = layoutMeasuredDimensionForAxis(node, axis);
    ! isUndefined(value) && value >= zero
  };
  let isLeadingPosDefined = (node, axis) =>
    isRowDirection(axis)
    && ! isUndefined(computedEdgeValuePosition(node.style, Start, cssUndefined))
    || ! isUndefined(computedEdgeValuePosition(node.style, leadingEdgeForAxis(axis), cssUndefined));
  let isTrailingPosDefined = (node, axis) =>
    isRowDirection(axis)
    && ! isUndefined(computedEdgeValuePosition(node.style, End, cssUndefined))
    || !
         isUndefined(
           computedEdgeValuePosition(node.style, trailingEdgeForAxis(axis), cssUndefined)
         );

  /***
   * The C implementation calls this `getLeadingPosition`.
   */
  let getLeadingPosition = (node, axis) =>
    if (isRowDirection(axis)) {
      let leadingPosition = computedEdgeValuePosition(node.style, Start, cssUndefined);
      if (! isUndefined(leadingPosition)) {
        leadingPosition
      } else {
        let leadingPosition =
          computedEdgeValuePosition(node.style, leadingEdgeForAxis(axis), cssUndefined);
        if (isUndefined(leadingPosition)) {
          zero
        } else {
          leadingPosition
        }
      }
    } else {
      let leadingPosition =
        computedEdgeValuePosition(node.style, leadingEdgeForAxis(axis), cssUndefined);
      if (isUndefined(leadingPosition)) {
        zero
      } else {
        leadingPosition
      }
    };

  /***
   * The C implementation calls this `getTrailingPosition`.
   */
  let getTrailingPosition = (node, axis) =>
    if (isRowDirection(axis)) {
      let trailingPosition = computedEdgeValuePosition(node.style, End, cssUndefined);
      if (! isUndefined(trailingPosition)) {
        trailingPosition
      } else {
        let trailingPosition =
          computedEdgeValuePosition(node.style, trailingEdgeForAxis(axis), cssUndefined);
        if (isUndefined(trailingPosition)) {
          zero
        } else {
          trailingPosition
        }
      }
    } else {
      let trailingPosition =
        computedEdgeValuePosition(node.style, trailingEdgeForAxis(axis), cssUndefined);
      if (isUndefined(trailingPosition)) {
        zero
      } else {
        trailingPosition
      }
    };
  let boundAxisWithinMinAndMax = (node, axis, value) => {
    let (min, max) =
      if (isColumnDirection(axis)) {
        (node.style.minHeight, node.style.maxHeight)
      } else if (isRowDirection(axis)) {
        (node.style.minWidth, node.style.maxWidth)
      } else {
        (cssUndefined, cssUndefined)
      };
    let boundValue = value;
    let nextBoundValue =
      if (! isUndefined(max) && max >= zero && boundValue > max) {
        max
      } else {
        boundValue
      };
    let nextNextBoundValue =
      if (! isUndefined(min)
          && min >= zero
          && ! isUndefined(nextBoundValue)
          && nextBoundValue < min) {
        min
      } else {
        nextBoundValue
      };
    nextNextBoundValue
  };
  /* Like boundAxisWithinMinAndMax but also ensures that the value doesn't go below the
   * padding and border amount. */
  let boundAxis = (node, axis, value) =>
    fmaxf(boundAxisWithinMinAndMax(node, axis, value), getPaddingAndBorderAxis(node, axis));
  /* /* When the user specifically sets a value for width or height */ */
  /* let setDimensionFromStyle (node, axis) => */
  /*   /* The parent already computed us a width or height. We just skip it */ */
  /*   if (isLayoutDimDefined (node, axis)) { */
  /*     () */
  /*   } else if */
  /*     /* We only run if there's a width or height defined */ */
  /*     (not (isStyleDimDefined (node, axis))) { */
  /*     () */
  /*   } else { */
  /*     /* The dimensions can never be smaller than the padding and border */ */
  /*     let dimValue = */
  /*       fmaxf (boundAxis (node, axis, node.style [dim [axis]])) (getPaddingAndBorderAxis (node, axis)); */
  /*     setDimLayoutDimensionForAxis node dimValue */
  /*   }; */

  /***
   * Sets trailing position for a child node for a given axis.
   */
  let setTrailingPosition = (node, child, axis) => {
    let size = layoutMeasuredDimensionForAxis(child, axis);
    let childLayoutPosValueForAxis = layoutPosPositionForAxis(child, axis);
    let nextValue =
      layoutMeasuredDimensionForAxis(node, axis) -. size -. childLayoutPosValueForAxis;
    setLayoutTrailingPositionForAxis(child, axis, nextValue)
  };
  /* If both left and right are defined, then use left. Otherwise return */
  /* +left or -right depending on which is defined. */
  let getRelativePosition = (node, axis) =>
    if (isLeadingPosDefined(node, axis)) {
      getLeadingPosition(node, axis)
    } else {
      -. getTrailingPosition(node, axis)
    };

  /***
   * TODO: A more functional version of this.
   */
  let setPosition = (node, direction) => {
    let (mainAxis, crossAxis) = resolveAxises(node.style.flexDirection, direction);
    let relativePositionMain = getRelativePosition(node, mainAxis);
    let relativePositionCross = getRelativePosition(node, crossAxis);
    setLayoutPositionForAxis(
      node,
      mainAxis,
      getLeadingMargin(node, mainAxis) +. relativePositionMain,
      getTrailingMargin(node, mainAxis) +. relativePositionMain
    );
    setLayoutPositionForAxis(
      node,
      crossAxis,
      getLeadingMargin(node, crossAxis) +. relativePositionCross,
      getTrailingMargin(node, crossAxis) +. relativePositionCross
    )
  };

  /***
   * We break up YGConstraintMaxSizeForMode into two separate functions so that
   * we don't have to use references that *leak* out of their scope. Refs that
   * don't leak out of their scope are optimized. Therefore we create two
   * functions that simply return a single value each.
   */
  let constrainSizeToMaxSizeForMode = (maxSize, mode, size) =>
    switch mode {
    | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS =>
      raise(Invalid_argument("No clue how this could happen"))
    | Exactly
    | AtMost =>
      if (isUndefined(maxSize) || size < maxSize) {
        size
      } else {
        maxSize
      }
    | Undefined =>
      if (! isUndefined(maxSize)) {
        maxSize
      } else {
        size
      }
    /* | Count => size */
    };
  let constrainModeToMaxSizeForMode = (maxSize, mode) =>
    switch mode {
    | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS =>
      raise(Invalid_argument("No clue how this could happen"))
    | Exactly
    | AtMost => mode
    | Undefined =>
      if (! isUndefined(maxSize)) {
        AtMost
      } else {
        mode
      }
    /* | Count => size */
    };
  let nodeWithMeasureFuncSetMeasuredDimensions =
      (node, availableWidth, availableHeight, widthMeasureMode, heightMeasureMode) =>
    switch node.measure {
    | None => raise(Invalid_argument(nodeWithNoMeasureStr))
    | Some(measure) =>
      let paddingAndBorderAxisRow = getPaddingAndBorderAxis(node, Row);
      let paddingAndBorderAxisColumn = getPaddingAndBorderAxis(node, Column);
      let marginAxisRow = getMarginAxis(node, Row);
      let marginAxisColumn = getMarginAxis(node, Column);
      let innerWidth = availableWidth -. marginAxisRow -. paddingAndBorderAxisRow;
      let innerHeight = availableHeight -. marginAxisColumn -. paddingAndBorderAxisColumn;
      if (widthMeasureMode === Exactly && heightMeasureMode === Exactly) {
        /* Don't bother sizing the text if both dimensions are already defined. */
        node.layout.measuredWidth = boundAxis(node, Row, availableWidth -. marginAxisRow);
        node.layout.measuredHeight = boundAxis(node, Column, availableHeight -. marginAxisColumn)
      } else if (! isUndefined(innerWidth)
                 && innerWidth <= zero
                 || ! isUndefined(innerHeight)
                 && innerHeight <= zero) {
        /* Don't bother sizing text if there's no horizontal or vertical space.  */
        node.layout.measuredWidth = boundAxis(node, Row, zero);
        node.layout.measuredHeight = boundAxis(node, Column, zero)
      } else {
        /* Measure the text under the current constraints */
        let measuredSize =
          measure(node, innerWidth, widthMeasureMode, innerHeight, heightMeasureMode);
        node.layout.measuredWidth =
          boundAxis(
            node,
            Row,
            if (widthMeasureMode === Undefined || widthMeasureMode === AtMost) {
              measuredSize.width +. paddingAndBorderAxisRow
            } else {
              availableWidth -. marginAxisRow
            }
          );
        node.layout.measuredHeight =
          boundAxis(
            node,
            Column,
            if (heightMeasureMode === Undefined || heightMeasureMode === AtMost) {
              measuredSize.height +. paddingAndBorderAxisColumn
            } else {
              availableHeight -. marginAxisColumn
            }
          )
      }
    };

  /***
   * For nodes with no children, use the available values if they were
   * provided, or the minimum size as indicated by the padding and border
   * sizes.
   */
  let emptyContainerSetMeasuredDimensions =
      (node, availableWidth, availableHeight, widthMeasureMode, heightMeasureMode) => {
    let paddingAndBorderAxisRow = getPaddingAndBorderAxis(node, Row);
    let paddingAndBorderAxisColumn = getPaddingAndBorderAxis(node, Column);
    let marginAxisRow = getMarginAxis(node, Row);
    let marginAxisColumn = getMarginAxis(node, Column);
    let boundWidthBy =
      if (widthMeasureMode === Undefined || widthMeasureMode === AtMost) {
        paddingAndBorderAxisRow
      } else {
        availableWidth -. marginAxisRow
      };
    node.layout.measuredWidth = boundAxis(node, Row, boundWidthBy);
    let boundHeightBy =
      if (heightMeasureMode === Undefined || heightMeasureMode === AtMost) {
        paddingAndBorderAxisColumn
      } else {
        availableHeight -. marginAxisColumn
      };
    node.layout.measuredHeight = boundAxis(node, Column, boundHeightBy)
  };
  let fixedSizeSetMeasuredDimensions =
      (node, availableWidth, availableHeight, widthMeasureMode, heightMeasureMode) =>
    if (widthMeasureMode === AtMost
        && ! isUndefined(availableWidth)
        && availableWidth <= zero
        || heightMeasureMode === AtMost
        && ! isUndefined(availableHeight)
        && availableHeight <= zero
        || widthMeasureMode === Exactly
        && heightMeasureMode === Exactly) {
      let marginAxisColumn = getMarginAxis(node, Column);
      let marginAxisRow = getMarginAxis(node, Row);
      let boundWidthBy =
        if (isUndefined(availableWidth) || widthMeasureMode === AtMost && availableWidth < zero) {
          zero
        } else {
          availableWidth -. marginAxisRow
        };
      node.layout.measuredWidth = boundAxis(node, Row, boundWidthBy);
      let boundHeightBy =
        if (isUndefined(availableHeight) || heightMeasureMode === AtMost && availableHeight < zero) {
          zero
        } else {
          availableHeight -. marginAxisColumn
        };
      node.layout.measuredHeight = boundAxis(node, Column, boundHeightBy);
      true
    } else {
      false
    };
};
