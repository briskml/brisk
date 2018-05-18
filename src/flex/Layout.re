/***
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
/*
 * From css-layout comments:
 * The spec describes four different layout modes: "fill available", "max
 * content", "min content", and "fit content". Of these, we don't use
 * "min content" because we don't support default minimum main sizes (see
 * above for details). Each of our measure modes maps to a layout mode
 * from the spec (https://www.w3.org/TR/css3-sizing/#terms):
 *
 *   -.Undefined: `max-content`
 *   -.Exactly: `fill-available`
 *   -.AtMost: `fit-content`
 *      If infinite space available in that axis, then `max-content.`
 *      Else, `min(max-content size, max(min-content size, fill-available size))`
 *      (Although, we don't support min-content)
 */
module Create = (Node: Spec.Node, Encoding: Spec.Encoding) => {
  module LayoutSupport = LayoutSupport.Create(Node, Encoding);
  module LayoutPrint = LayoutPrint.Create(Node, Encoding);
  open LayoutSupport.LayoutTypes;
  open LayoutSupport;
  /* open Encoding; */
  open HardCodedEncoding;
  let gCurrentGenerationCount = ref(0);
  let gDepth = ref(0);
  let gPrintTree = {contents: false};
  let gPrintChanges = {contents: false};
  let gPrintSkips = {contents: false};
  let measureString = "measure";
  let stretchString = "stretch";
  let absMeasureString = "abs-measure";
  let absLayoutString = "abs-layout";
  let initialString = "initial";
  let flexString = "flex";
  let spacer = "                                                            ";
  let pMeasureMode =
    fun
    | Undefined => "Undefined"
    | Exactly => "Exactly" /* 'exactly' */
    | AtMost => "AtMost" /* 'at-most' */
    | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS => "-1";
  let getSpacer = (level) => {
    let spacerLen = String.length(spacer);
    let lvl = level > spacerLen ? level : spacerLen;
    String.sub(spacer, lvl, String.length(spacer))
  };
  let getModeName = ((mode, isLayoutInsteadOfMeasure)) =>
    switch mode {
    | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS =>
      isLayoutInsteadOfMeasure ?
        "CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS" :
        "CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS"
    | Undefined => isLayoutInsteadOfMeasure ? "LAY_UNDEFINED" : "UNDEFINED"
    | Exactly => isLayoutInsteadOfMeasure ? "LAY_EXACTLY" : "EXACTLY"
    | AtMost => isLayoutInsteadOfMeasure ? "LAY_AT_MOST" : "AT_MOST"
    };
  let canUseCachedMeasurement =
      (
        availableWidth,
        availableHeight,
        marginRow,
        marginColumn,
        widthMeasureMode,
        heightMeasureMode,
        cachedLayout
      ) =>
    if (cachedLayout.availableWidth == availableWidth
        && cachedLayout.availableHeight == availableHeight
        && cachedLayout.widthMeasureMode == widthMeasureMode
        && cachedLayout.heightMeasureMode == heightMeasureMode) {
      true
    } else if
      /* Is it an exact match?*/
      /* If the width is an exact match, try a fuzzy match on the height.*/
      (cachedLayout.widthMeasureMode == widthMeasureMode
       && cachedLayout.availableWidth == availableWidth
       && heightMeasureMode === Exactly
       && availableHeight
       -. marginColumn == cachedLayout.computedHeight) {
      true
    } else if
      /* If the height is an exact match, try a fuzzy match on the width.*/
      (cachedLayout.heightMeasureMode == heightMeasureMode
       && cachedLayout.availableHeight == availableHeight
       && widthMeasureMode === Exactly
       && availableWidth
       -. marginRow == cachedLayout.computedWidth) {
      true
    } else {
      false
    };
  let cachedMeasurementAt = (layout, i) =>
    switch i {
    /* Lazily allocate measurement entry by doing referential equality check  */
    | 0 =>
      if (layout.cachedMeasurement1 === dummyCachedMeasurement) {
        layout.cachedMeasurement1 = createCacheMeasurement()
      };
      layout.cachedMeasurement1
    | 1 =>
      if (layout.cachedMeasurement2 === dummyCachedMeasurement) {
        layout.cachedMeasurement2 = createCacheMeasurement()
      };
      layout.cachedMeasurement2
    | 2 =>
      if (layout.cachedMeasurement3 === dummyCachedMeasurement) {
        layout.cachedMeasurement3 = createCacheMeasurement()
      };
      layout.cachedMeasurement3
    | 3 =>
      if (layout.cachedMeasurement4 === dummyCachedMeasurement) {
        layout.cachedMeasurement4 = createCacheMeasurement()
      };
      layout.cachedMeasurement4
    | 4 =>
      if (layout.cachedMeasurement5 === dummyCachedMeasurement) {
        layout.cachedMeasurement5 = createCacheMeasurement()
      };
      layout.cachedMeasurement5
    | 5 =>
      if (layout.cachedMeasurement6 === dummyCachedMeasurement) {
        layout.cachedMeasurement6 = createCacheMeasurement()
      };
      layout.cachedMeasurement6
    | _ => raise(Invalid_argument("No cached measurement at " ++ string_of_int(i)))
    };

  /***
   * This is a wrapper around the layoutNodeImpl function. It determines
   * whether the layout request is redundant and can be skipped.
   *
   * Parameters:
   *  Input parameters are the same as layoutNodeImpl (see above)
   *  Return parameter is true if layout was performed, false if skipped
   */
  let rec layoutNodeInternal =
          (
            node,
            availableWidth,
            availableHeight,
            parentDirection,
            widthMeasureMode,
            heightMeasureMode,
            performLayout,
            reason
          ) => {
    let layout = node.layout;
    gDepth.contents = gDepth.contents + 1;
    let needToVisitNode =
      node.isDirty
      && layout.generationCount != gCurrentGenerationCount.contents
      || layout.lastParentDirection != parentDirection;
    if (needToVisitNode) {
      /* Invalidate the cached results.*/
      layout.nextCachedMeasurementsIndex = 0;
      layout.cachedLayout.widthMeasureMode = CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS;
      layout.cachedLayout.heightMeasureMode = CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS;
      layout.cachedLayout.computedWidth = negativeOne;
      layout.cachedLayout.computedHeight = negativeOne
    };
    let cachedResults = ref(None);
    /* Determine whether the results are already cached. We maintain a separate*/
    /* cache for layouts and measurements. A layout operation modifies the positions*/
    /* and dimensions for nodes in the subtree. The algorithm assumes that each node*/
    /* gets layed out a maximum of one time per tree layout, but multiple measurements*/
    /* may be required to resolve all of the flex dimensions.*/
    /* We handle nodes with measure functions specially here because they are the most
     * expensive to measure, so it's worth avoiding redundant measurements if at all possible.*/
    if (node.measure !== None) {
      let marginAxisRow = getMarginAxis(node, Row);
      let marginAxisColumn = getMarginAxis(node, Column);
      /* First, try to use the layout cache.*/
      if (canUseCachedMeasurement(
            availableWidth,
            availableHeight,
            marginAxisRow,
            marginAxisColumn,
            widthMeasureMode,
            heightMeasureMode,
            layout.cachedLayout
          )) {
        cachedResults.contents = Some(layout.cachedLayout)
      } else {
        /* Try to use the measurement cache.*/
        let foundCached = {contents: false};
        for (i in 0 to layout.nextCachedMeasurementsIndex - 1) {
          /* This is basically the "break" */
          if (! foundCached.contents) {
            let cachedMeasurementAtIndex = cachedMeasurementAt(layout, i);
            if (canUseCachedMeasurement(
                  availableWidth,
                  availableHeight,
                  marginAxisRow,
                  marginAxisColumn,
                  widthMeasureMode,
                  heightMeasureMode,
                  cachedMeasurementAtIndex
                )) {
              cachedResults.contents = Some(cachedMeasurementAtIndex);
              foundCached.contents = true
            }
          }
        }
      }
    } else if (performLayout) {
      if (layout.cachedLayout.availableWidth == availableWidth
          && layout.cachedLayout.availableHeight == availableHeight
          && layout.cachedLayout.widthMeasureMode == widthMeasureMode
          && layout.cachedLayout.heightMeasureMode == heightMeasureMode) {
        cachedResults.contents = Some(layout.cachedLayout)
      }
    } else {
      let foundCached = {contents: false};
      for (i in 0 to layout.nextCachedMeasurementsIndex - 1) {
        /* This is basically the "break" */
        if (! foundCached.contents) {
          let cachedMeasurementAtIndex = cachedMeasurementAt(layout, i);
          if (cachedMeasurementAtIndex.availableWidth == availableWidth
              && cachedMeasurementAtIndex.availableHeight == availableHeight
              && cachedMeasurementAtIndex.widthMeasureMode == widthMeasureMode
              && cachedMeasurementAtIndex.heightMeasureMode == heightMeasureMode) {
            cachedResults.contents = Some(cachedMeasurementAtIndex);
            foundCached.contents = true
          }
        }
      }
    };
    if (! needToVisitNode && cachedResults.contents != None) {
      let cachedResults_ =
        switch cachedResults.contents {
        | None => raise(Invalid_argument("Not possible"))
        | Some(cr) => cr
        };
      layout.measuredWidth = cachedResults_.computedWidth;
      layout.measuredHeight = cachedResults_.computedHeight;
      if (gPrintChanges.contents && gPrintSkips.contents) {
        Printf.printf("%s%d.{[skipped] ", getSpacer(gDepth.contents), gDepth.contents);
        switch node.print {
        | None => ()
        | Some(printer) => printer(node.context)
        };
        Printf.printf(
          "wm: %s, hm: %s, aw: %s ah: %s => d: (%s, %s) %s\n",
          getModeName((widthMeasureMode, performLayout)),
          getModeName((heightMeasureMode, performLayout)),
          scalarToString(availableWidth),
          scalarToString(availableHeight),
          scalarToString(cachedResults_.computedWidth),
          scalarToString(cachedResults_.computedHeight),
          reason
        )
      }
    } else {
      if (gPrintChanges.contents) {
        Printf.printf(
          "%s%d.{%s",
          getSpacer(gDepth.contents),
          gDepth.contents,
          needToVisitNode ? "*" : ""
        );
        switch node.print {
        | None => ()
        | Some(printer) => printer(node.context)
        };
        Printf.printf(
          "wm: %s, hm: %s, aw: %s ah: %s %s\n",
          getModeName((widthMeasureMode, performLayout)),
          getModeName((heightMeasureMode, performLayout)),
          scalarToString(availableWidth),
          scalarToString(availableHeight),
          reason
        )
      };
      layoutNodeImpl(
        node,
        availableWidth,
        availableHeight,
        parentDirection,
        widthMeasureMode,
        heightMeasureMode,
        performLayout
      );
      if (gPrintChanges.contents) {
        Printf.printf(
          "%s%d.}%s",
          getSpacer(gDepth.contents),
          gDepth.contents,
          needToVisitNode ? "*" : ""
        );
        switch node.print {
        | None => ()
        | Some(printer) => printer(node.context)
        };
        Printf.printf(
          "wm: %s, hm: %s, d: (%s, %s) %s\n",
          getModeName((widthMeasureMode, performLayout)),
          getModeName((heightMeasureMode, performLayout)),
          scalarToString(layout.measuredWidth),
          scalarToString(layout.measuredHeight),
          reason
        )
      };
      layout.lastParentDirection = parentDirection;
      if (cachedResults.contents === None) {
        if (layout.nextCachedMeasurementsIndex == css_max_cached_result_count) {
          if (gPrintChanges.contents) {
            Printf.printf("Out of cache entries!\n")
          };
          layout.nextCachedMeasurementsIndex = 0
        };
        let newCacheEntry =
          performLayout ?
            /* Use the single layout cache entry.*/
            layout.cachedLayout :
            {
              /* Allocate a new measurement cache entry.*/
              let newCacheEntry_ = cachedMeasurementAt(layout, layout.nextCachedMeasurementsIndex);
              layout.nextCachedMeasurementsIndex = layout.nextCachedMeasurementsIndex + 1;
              newCacheEntry_
            };
        newCacheEntry.availableWidth = availableWidth;
        newCacheEntry.availableHeight = availableHeight;
        newCacheEntry.widthMeasureMode = widthMeasureMode;
        newCacheEntry.heightMeasureMode = heightMeasureMode;
        newCacheEntry.computedWidth = layout.measuredWidth;
        newCacheEntry.computedHeight = layout.measuredHeight
      }
    };
    if (performLayout) {
      node.layout.width = node.layout.measuredWidth;
      node.layout.height = node.layout.measuredHeight;
      node.hasNewLayout = true;
      node.isDirty = false
    };
    gDepth.contents = gDepth.contents - 1;
    layout.generationCount = gCurrentGenerationCount.contents;
    needToVisitNode || cachedResults.contents === None
  }
  and isExperimentalFeatureEnabled = (_) => false
  and computeChildFlexBasis = (node, child, width, widthMode, height, heightMode, direction) => {
    let mainAxis = resolveAxis(node.style.flexDirection, direction);
    let isMainAxisRow = isRowDirection(mainAxis);
    let childWidth = {contents: zero};
    let childHeight = {contents: zero};
    let childWidthMeasureMode = {contents: Undefined};
    let childHeightMeasureMode = {contents: Undefined};
    let isRowStyleDimDefined = isStyleDimDefined(child, Row);
    let isColumnStyleDimDefined = isStyleDimDefined(child, Column);
    let thisChildFlexBasis = LayoutSupport.cssGetFlexBasis(child);
    if (! isUndefined(thisChildFlexBasis)
        && ! isUndefined(if (isMainAxisRow) {width} else {height})) {
      if (isUndefined(child.layout.computedFlexBasis)
          || isExperimentalFeatureEnabled()
          && child.layout.computedFlexBasisGeneration !== gCurrentGenerationCount.contents) {
        child.layout.computedFlexBasis =
          fmaxf(thisChildFlexBasis, getPaddingAndBorderAxis(child, mainAxis))
      }
    } else if (isMainAxisRow && isRowStyleDimDefined) {
      /* The width is definite, so use that as the flex basis. */
      child.layout.computedFlexBasis =
        fmaxf(child.style.width, getPaddingAndBorderAxis(child, Row))
    } else if (! isMainAxisRow && isColumnStyleDimDefined) {
      /* The height is definite, so use that as the flex basis. */
      child.layout.computedFlexBasis =
        fmaxf(child.style.height, getPaddingAndBorderAxis(child, Column))
    } else {
      childWidth.contents = cssUndefined;
      childHeight.contents = cssUndefined;
      childWidthMeasureMode.contents = Undefined;
      childHeightMeasureMode.contents = Undefined;
      if (isRowStyleDimDefined) {
        childWidth.contents = child.style.width +. getMarginAxis(child, Row);
        childWidthMeasureMode.contents = Exactly
      };

      /***
       * Why can't this just be inlined to .height !== cssUndefined.
       */
      if (isColumnStyleDimDefined) {
        childHeight.contents = child.style.height +. getMarginAxis(child, Column);
        childHeightMeasureMode.contents = Exactly
      };
      /* The W3C spec doesn't say anything about the 'overflow' property, */
      /* but all major browsers appear to implement the following logic. */
      if (! isMainAxisRow && node.style.overflow === Scroll || node.style.overflow !== Scroll) {
        if (isUndefined(childWidth.contents) && ! isUndefined(width)) {
          childWidth.contents = width;
          childWidthMeasureMode.contents = AtMost
        }
      };
      if (isMainAxisRow && node.style.overflow === Scroll || node.style.overflow !== Scroll) {
        if (isUndefined(childHeight.contents) && ! isUndefined(height)) {
          childHeight.contents = height;
          childHeightMeasureMode.contents = AtMost
        }
      };
      /*
       * If child has no defined size in the cross axis and is set to
       * stretch, set the cross axis to be measured exactly with the
       * available inner width.
       */
      if (! isMainAxisRow
          && ! isUndefined(width)
          && ! isRowStyleDimDefined
          && widthMode === Exactly
          && getAlignItem(node, child) === AlignStretch) {
        childWidth.contents = width;
        childWidthMeasureMode.contents = Exactly
      };
      if (isMainAxisRow
          && ! isUndefined(height)
          && ! isColumnStyleDimDefined
          && heightMode === Exactly
          && getAlignItem(node, child) === AlignStretch) {
        childHeight.contents = height;
        childHeightMeasureMode.contents = Exactly
      };
      /* if (!YGValueIsUndefined(child->style.aspectRatio)) { */
      /*   if (!isMainAxisRow && childWidthMeasureMode == YGMeasureModeExactly) { */
      /*     child->layout.computedFlexBasis = */
      /*         fmaxf(childWidth * child->style.aspectRatio, */
      /*               YGNodePaddingAndBorderForAxis(child, YGFlexDirectionColumn)); */
      /*     return; */
      /*   } else if (isMainAxisRow && childHeightMeasureMode == YGMeasureModeExactly) { */
      /*     child->layout.computedFlexBasis = */
      /*         fmaxf(childHeight * child->style.aspectRatio, */
      /*               YGNodePaddingAndBorderForAxis(child, YGFlexDirectionRow)); */
      /*     return; */
      /*   } */
      /* } */
      childWidth.contents =
        constrainSizeToMaxSizeForMode(
          child.style.maxWidth,
          childWidthMeasureMode.contents,
          childWidth.contents
        );
      childWidthMeasureMode.contents =
        constrainModeToMaxSizeForMode(child.style.maxWidth, childWidthMeasureMode.contents);
      childHeight.contents =
        constrainSizeToMaxSizeForMode(
          child.style.maxHeight,
          childHeightMeasureMode.contents,
          childHeight.contents
        );
      childHeightMeasureMode.contents =
        constrainModeToMaxSizeForMode(child.style.maxHeight, childHeightMeasureMode.contents);
      /* Measure the child */
      let _ =
        layoutNodeInternal(
          child,
          childWidth.contents,
          childHeight.contents,
          direction,
          childWidthMeasureMode.contents,
          childHeightMeasureMode.contents,
          false,
          measureString
        );
      child.layout.computedFlexBasis =
        fmaxf(
          if (isMainAxisRow) {
            child.layout.measuredWidth
          } else {
            child.layout.measuredHeight
          },
          getPaddingAndBorderAxis(child, mainAxis)
        )
    };
    child.layout.computedFlexBasisGeneration = gCurrentGenerationCount.contents
  }
  /***
   * @child The child with absolute position.
   * @width The available inner width.
   */
  and absoluteLayoutChild = (node, child, width, widthMode, direction) => {
    let (mainAxis, crossAxis) = resolveAxises(node.style.flexDirection, direction);
    let childWidth = {contents: cssUndefined};
    let childHeight = {contents: cssUndefined};
    let childWidthMeasureMode = {contents: Undefined};
    let childHeightMeasureMode = {contents: Undefined};
    let isMainAxisRow = isRowDirection(mainAxis);
    if (isStyleDimDefined(child, Row)) {
      childWidth.contents = child.style.width +. getMarginAxis(child, Row)
    } else if (isLeadingPosDefined(child, Row) && isTrailingPosDefined(child, Row)) {
      childWidth.contents =
        node.layout.measuredWidth
        -. (getLeadingBorder(node, Row) +. getTrailingBorder(node, Row))
        -. (getLeadingPosition(child, Row) +. getTrailingPosition(child, Row));
      childWidth.contents = boundAxis(child, Row, childWidth.contents)
    };
    if (isStyleDimDefined(child, Column)) {
      childHeight.contents = child.style.height +. getMarginAxis(child, Column)
    } else if
      /* If the child doesn't have a specified height, compute the height based on the top/bottom offsets if they're defined. */
      (isLeadingPosDefined(child, Column) && isTrailingPosDefined(child, Column)) {
      childHeight.contents =
        node.layout.measuredHeight
        -. (getLeadingBorder(node, Column) +. getTrailingBorder(node, Column))
        -. (getLeadingPosition(child, Column) +. getTrailingPosition(child, Column));
      childHeight.contents = boundAxis(child, Column, childHeight.contents)
    };

    /***
     * TODO: Insert some aspect ratio logic here.
     */
    if (isUndefined(childWidth.contents) || isUndefined(childHeight.contents)) {
      childWidthMeasureMode.contents = (
        if (isUndefined(childWidth.contents)) {
          Undefined
        } else {
          Exactly
        }
      );
      childHeightMeasureMode.contents = (
        if (isUndefined(childHeight.contents)) {
          Undefined
        } else {
          Exactly
        }
      );
      /*
       * According to the spec, if the main size is not definite and the
       * child's inline axis is parallel to the main axis (i.e. it's
       * horizontal), the child should be sized using "UNDEFINED" in
       * the main size. Otherwise use "AT_MOST" in the cross axis.
       */
      if (! isMainAxisRow && isUndefined(childWidth.contents) && widthMode !== Undefined) {
        childWidth.contents = width;
        childWidthMeasureMode.contents = AtMost
      };
      /*
       * If child has no defined size in the cross axis and is set to stretch, set the cross
       * axis to be measured exactly with the available inner width
       */
      let _ =
        layoutNodeInternal(
          child,
          childWidth.contents,
          childHeight.contents,
          direction,
          childWidthMeasureMode.contents,
          childHeightMeasureMode.contents,
          false,
          absMeasureString
        );
      childWidth.contents = child.layout.measuredWidth +. getMarginAxis(child, Row);
      childHeight.contents = child.layout.measuredHeight +. getMarginAxis(child, Column)
    };
    let _ =
      layoutNodeInternal(
        child,
        childWidth.contents,
        childHeight.contents,
        direction,
        Exactly,
        Exactly,
        true,
        absLayoutString
      );
    if (isTrailingPosDefined(child, mainAxis) && ! isLeadingPosDefined(child, mainAxis)) {
      setLayoutLeadingPositionForAxis(
        child,
        mainAxis,
        layoutMeasuredDimensionForAxis(node, mainAxis)
        -. layoutMeasuredDimensionForAxis(child, mainAxis)
        -. getTrailingBorder(node, mainAxis)
        -. getTrailingPosition(child, mainAxis)
      )
    };
    if (isTrailingPosDefined(child, crossAxis) && ! isLeadingPosDefined(child, crossAxis)) {
      setLayoutLeadingPositionForAxis(
        child,
        crossAxis,
        layoutMeasuredDimensionForAxis(node, crossAxis)
        -. layoutMeasuredDimensionForAxis(child, crossAxis)
        -. getTrailingBorder(node, crossAxis)
        -. getTrailingPosition(child, crossAxis)
      )
    }
  }
  /***
   * By default, mathematical operations are floating point.
   */
  and layoutNodeImpl =
      (
        node,
        availableWidth,
        availableHeight,
        parentDirection,
        widthMeasureMode,
        heightMeasureMode,
        performLayout
      ) => {

    /*** START_GENERATED **/
    assert (isUndefined(availableWidth) ? widthMeasureMode === Undefined : true);
    assert (isUndefined(availableHeight) ? heightMeasureMode === Undefined : true);
    let direction = resolveDirection(node, parentDirection);
    if (node.layout.direction !== direction) {
      node.layout.direction = direction
    };
    /* For content (text) nodes, determine the dimensions based on the text
       contents. */
    if (node.measure !== None) {
      nodeWithMeasureFuncSetMeasuredDimensions(
        node,
        availableWidth,
        availableHeight,
        widthMeasureMode,
        heightMeasureMode
      )
    } else if (node.childrenCount === 0) {
      emptyContainerSetMeasuredDimensions(
        node,
        availableWidth,
        availableHeight,
        widthMeasureMode,
        heightMeasureMode
      )
    } else {
      let res =
        ! performLayout
        && fixedSizeSetMeasuredDimensions(
             node,
             availableWidth,
             availableHeight,
             widthMeasureMode,
             heightMeasureMode
           );
      if (! res) {
        /* STEP 1: CALCULATE VALUES FOR REMAINDER OF ALGORITHM */
        let (mainAxis, crossAxis) = resolveAxises(node.style.flexDirection, direction);
        let isMainAxisRow = isRowDirection(mainAxis);
        let justifyContent = node.style.justifyContent;
        let isNodeFlexWrap = node.style.flexWrap === CssWrap;
        let firstAbsoluteChild = {contents: theNullNode};
        let currentAbsoluteChildRef = {contents: theNullNode};

        /*** Padding and border. */
        let leadingPaddingAndBorderMain = getLeadingPaddingAndBorder(node, mainAxis);
        let trailingPaddingAndBorderMain = getTrailingPaddingAndBorder(node, mainAxis);
        let leadingPaddingAndBorderCross = getLeadingPaddingAndBorder(node, crossAxis);

        /*** Padding/border axis */
        let paddingAndBorderAxisMain = getPaddingAndBorderAxis(node, mainAxis);
        let paddingAndBorderAxisCross = getPaddingAndBorderAxis(node, crossAxis);

        /*** Measure mode */
        let measureModeMainDim = if (isMainAxisRow) {widthMeasureMode} else {heightMeasureMode};
        let measureModeCrossDim = if (isMainAxisRow) {heightMeasureMode} else {widthMeasureMode};

        /*** Padding/border/ margin row/column axis */
        let paddingAndBorderAxisRow =
          if (isMainAxisRow) {paddingAndBorderAxisMain} else {paddingAndBorderAxisCross};
        let paddingAndBorderAxisColumn =
          if (isMainAxisRow) {paddingAndBorderAxisCross} else {paddingAndBorderAxisMain};
        let marginAxisRow = getMarginAxis(node, Row);
        let marginAxisColumn = getMarginAxis(node, Column);

        /*** STEP 2: DETERMINE AVAILABLE SIZE IN MAIN AND CROSS DIRECTIONS */
        let availableInnerWidth = availableWidth -. marginAxisRow -. paddingAndBorderAxisRow;
        let availableInnerHeight =
          availableHeight -. marginAxisColumn -. paddingAndBorderAxisColumn;
        let availableInnerMainDim =
          if (isMainAxisRow) {availableInnerWidth} else {availableInnerHeight};
        let availableInnerCrossDim =
          if (isMainAxisRow) {availableInnerHeight} else {availableInnerWidth};
        let child = {contents: theNullNode};
        let childCount = node.childrenCount;

        /***
         * If there is only one child with flexGrow + flexShrink it means we
         * can set the computedFlexBasis to 0 instead of measuring and
         * shrinking / flexing the child to exactly match the remaining space.
         */
        let singleFlexChild = {contents: theNullNode};
        if (isMainAxisRow
            && widthMeasureMode == Exactly
            || ! isMainAxisRow
            && heightMeasureMode == Exactly) {
          let shouldContinue = {contents: true};
          let i = {contents: 0};
          while (shouldContinue.contents && i.contents < childCount) {
            let child = node.children[i.contents];
            if (singleFlexChild.contents !== theNullNode) {
              if (isFlex(child)) {
                /* There is already a flexible child, abort. */
                singleFlexChild.contents = theNullNode;
                shouldContinue.contents = false
              }
            } else if
              /* TODO: fixedPoint guard against cssUndefined when testing w > and <
                 This specific case is fine but only coincidentally. */
              (cssGetFlexGrow(child) > zero && cssGetFlexShrink(child) > zero) {
              singleFlexChild.contents = child
            };
            i.contents = i.contents + 1
          }
        };
        /* STEP 3: DETERMINE FLEX BASIS FOR EACH ITEM */

        /***
         * This loop computes `.computedFlexBasis` for each child, and
         * constructs a chain of absolute children using their `.nextChild`
         * property.
         *
         * For every non-absolute child, we store a `.computedFlexBasis`.
         *
         * Later, after this for loop, we construct a chain of relative
         * children.
         *
         * We should find a way to avoid having to construct these chains or
         * store a computed flex basis at all. Perhaps we can show that for each
         * node it's only used once, and therefore can be computed when needed,
         * resulting in just as many computations, but zero mutations.
         *
         * Insight: Most children in practice are either all absolute or all
         * relative. Maybe there's a way to optimize for that. If so, we could
         * then just do straight loops over the children, and not mess with
         * `nextChild` pointers.
         */
        for (i in 0 to childCount - 1) {
          let child = node.children[i];
          assert (child !== theNullNode);
          if (performLayout) {
            /* This is strange. We set the layout values to some intermediate
             * ones based purely on the style (essentially marginLeft + left).
             * This doesn't take into account the flex algorithm at all at thi
             * point, only the style.
             *
             * I think this intermediate position is either supposed to be
             * relative to the parent (if it's position:absolute) or relative to
             * its default layout (position:relative).
             */
            let childDirection = resolveDirection(child, direction);
            setPosition(child, childDirection)
          };
          if (child.style.positionType === Absolute) {
            if (firstAbsoluteChild.contents === theNullNode) {
              firstAbsoluteChild.contents = child
            };
            let previousAbsoluteChild = currentAbsoluteChildRef.contents;
            /* If there was a prev absolute, set its `nextChild` to `child`. */
            if (previousAbsoluteChild !== theNullNode) {
              /* TODO: only set nextChild when it would differ. */
              previousAbsoluteChild.nextChild = child
            };
            currentAbsoluteChildRef.contents = child
          } else if (child === singleFlexChild.contents) {
            child.layout.computedFlexBasisGeneration = gCurrentGenerationCount.contents;
            child.layout.computedFlexBasis = zero
          } else {
            /***
             * We compute the flex basis for each child, then just read it out
             * immediately.  That might be okay, but it's not clear if we read
             * that value out more than once for any given child. If not, we
             * should only compute it on the fly and not pay the cost of
             * mutation. This mutation happens *all* the time.
             */
            computeChildFlexBasis(
              node,
              child,
              availableInnerWidth,
              widthMeasureMode,
              availableInnerHeight,
              heightMeasureMode,
              direction
            )
          }
        };
        if (currentAbsoluteChildRef.contents !== theNullNode) {
          /* Set `child.nextChild` to null in case it's the last absolute */
          /* TODO: only set nextChild when it would differ. */
          currentAbsoluteChildRef.contents.nextChild = theNullNode
        };
        /* STEP 4: COLLECT FLEX ITEMS INTO FLEX LINES */
        let startOfLineIndex = {contents: 0};
        let endOfLineIndex = {contents: 0};
        let lineCount = {contents: 0};
        let totalLineCrossDim = {contents: zero};
        let maxLineMainDim = {contents: zero};
        while (endOfLineIndex.contents < childCount) {
          /* Number of items on the current line. May be different than the
           * difference between start and end indicates because we skip over
           * absolute-positioned items. */
          let itemsOnLine = {contents: 0};
          /* sizeConsumedOnCurrentLine is accumulation of the dimensions and
           * margin of all the children on the current line. This will be used
           * in order to either set the dimensions of the node if none already
           * exist or to compute the remaining space left for the flexible
           * children. */
          let sizeConsumedOnCurrentLine = {contents: zero};
          let totalFlexGrowFactors = {contents: zero};
          let totalFlexShrinkScaledFactors = {contents: zero};
          let curIndex = {contents: startOfLineIndex.contents};

          /*** Maintain a linked list of the child nodes that can shrink and/or grow. */
          let firstRelativeChild = {contents: theNullNode};
          let currentRelativeChild = {contents: theNullNode};
          let shouldContinue = {contents: true};
          /* Add items to the current line until it's full or we run out of items. */
          while (curIndex.contents < childCount && shouldContinue.contents) {
            child.contents = node.children[curIndex.contents];
            /* TODO: only set nextChild when it would differ. (But consider
             * effects on initial render) */
            child.contents.lineIndex = lineCount.contents;
            if (child.contents.style.positionType !== Absolute) {
              let outerFlexBasis =
                child.contents.layout.computedFlexBasis +. getMarginAxis(child.contents, mainAxis);
              /* If this is a multi-line flow and this item pushes us over the
               * available size, we've hit the end of the current line. Break
               * out of the loop and lay out the current line. */
              /* #different: Perform isNodeFlexWrap first! */
              let isEndOfLine =
                isNodeFlexWrap
                && sizeConsumedOnCurrentLine.contents
                +. outerFlexBasis > availableInnerMainDim
                && itemsOnLine.contents > 0;
              if (isEndOfLine) {
                /* There must be *no* increments in this code path */
                shouldContinue.contents =
                  false
              } else {
                sizeConsumedOnCurrentLine.contents =
                  sizeConsumedOnCurrentLine.contents +. outerFlexBasis;
                itemsOnLine.contents = itemsOnLine.contents + 1;
                if (isFlex(child.contents)) {
                  totalFlexGrowFactors.contents =
                    totalFlexGrowFactors.contents +. cssGetFlexGrow(child.contents);
                  /* Unlike the grow factor, the shrink factor is scaled
                   * relative to the child dimension. */
                  totalFlexShrinkScaledFactors.contents =
                    totalFlexShrinkScaledFactors.contents
                    +. -. cssGetFlexShrink(child.contents)
                    *. child.contents.layout.computedFlexBasis
                };
                if (firstRelativeChild.contents === theNullNode) {
                  firstRelativeChild.contents = child.contents
                };
                if (currentRelativeChild.contents !== theNullNode) {
                  currentRelativeChild.contents.nextChild = child.contents
                };
                currentRelativeChild.contents = child.contents;
                child.contents.nextChild = theNullNode;
                curIndex.contents = curIndex.contents + 1;
                endOfLineIndex.contents = endOfLineIndex.contents + 1
              }
            } else {
              /* The increment is repeated in a strange way because it's
               * transcribed from a C loop with a break. */
              curIndex.contents = curIndex.contents + 1;
              endOfLineIndex.contents = endOfLineIndex.contents + 1
            }
          };
          /* If we don't need to measure the cross axis, we can skip the entire
           * flex step. */
          let canSkipFlex = ! performLayout && measureModeCrossDim === Exactly;

          /***
           * In order to position the elements in the main axis, we have two
           * controls. The space between the beginning and the first element
           * and the space between each two elements.
           *
           * - leadingMainDim
           * - betweenMainDim
           */

          /*** STEP 5: RESOLVING FLEXIBLE LENGTHS ON MAIN AXIS
           * Calculate the remaining available space that needs to be
           * allocated. If the main dimension size isn't known, it is computed
           * based on the line length, so there's no more space left to
           * distribute. */
          let remainingFreeSpace = {contents: zero};
          if (! isUndefined(availableInnerMainDim)) {
            remainingFreeSpace.contents =
              availableInnerMainDim -. sizeConsumedOnCurrentLine.contents
          } else if (! isUndefined(sizeConsumedOnCurrentLine.contents)
                     && sizeConsumedOnCurrentLine.contents < zero) {
            /* availableInnerMainDim is indefinite which means the node is
             * being sized based on its content. sizeConsumedOnCurrentLine is
             * negative which means the node will allocate 0 pixels for its
             * content. Consequently, remainingFreeSpace is 0 -
             * sizeConsumedOnCurrentLine. */
            remainingFreeSpace.contents =
              -. sizeConsumedOnCurrentLine.contents
          };
          let originalRemainingFreeSpace = remainingFreeSpace.contents;
          let deltaFreeSpace = {contents: zero};

          /***
           * For the following markup, there is a bug.
           *
           * The `computedFlexBasis` of the first child is `20000`, and the
           * `computedFlexBasis` of the second is `18000` (which is the total
           * `20000` - `2 * 10`(padding)). That computed flex basis is likely
           * wrong (or not?) but it leaves `originalRemainingFreeSpace` =
           * `-2000`. If correct, it's not getting distributed to the second
           * child because the second child only has `flexGrow` (not shrink).
           * But I *don't* think it's correct. If no flex basis/width is
           * explicitly set in the style, I think it should look at te
           * width/height as described here:
           * https://developer.mozilla.org/en-US/docs/Web/CSS/flex-basis
           *
           *  <div id='bugRepro2_simplified' style='padding-start: 10px; padding-end:10px; width:200px; flex-direction: row;'>
           *    <div style='justify-content:flex-start; width:20px;'></div>
           *    <div style='min-height: 0px; flex-direction: row; flex-grow: 1;'>
           *      <div style='flex-grow: 1;'> </div>
           *    </div>
           *  </div>
           */
          if (! canSkipFlex) {
            /* Do two passes over the flex items to figure out how to
             * distribute the remaining space.  The first pass finds the items
             * whose min/max constraints trigger, freezes them at those sizes,
             * and excludes those sizes from the remaining space. The second
             * pass sets the size of each flexible item. It distributes the
             * remaining space amongst the items whose min/max constraints
             * didn't trigger in pass 1. For the other items, it sets their
             * sizes by forcing their min/max constraints to trigger again.
             *
             * This two pass approach for resolving min/max constraints
             * deviates from the spec. The spec
             * (https://www.w3.org/TR/YG-flexbox-1/#resolve-flexible-lengths)
             * describes a process that needs to be repeated a variable number
             * of times. The algorithm implemented here won't handle all cases
             * but it was simpler to implement and it mitigates performance
             * concerns because we know exactly how many passes it'll do. */
            let deltaFlexShrinkScaledFactors = {contents: zero};
            let deltaFlexGrowFactors = {contents: zero};
            currentRelativeChild.contents = firstRelativeChild.contents;
            while (currentRelativeChild.contents !== theNullNode) {
              let childFlexBasis = currentRelativeChild.contents.layout.computedFlexBasis;
              if (remainingFreeSpace.contents < zero) {
                let flexShrinkScaledFactor =
                  -. cssGetFlexShrink(currentRelativeChild.contents) *. childFlexBasis;
                /* Is this child able to shrink? */
                if (flexShrinkScaledFactor != zero) {
                  let baseMainSize =
                    childFlexBasis
                    /*
                     * Important to first scale, then divide - to support fixed
                     * point encoding.
                     */
                    +. flexShrinkScaledFactor
                    *. remainingFreeSpace.contents
                    /. totalFlexShrinkScaledFactors.contents;
                  let boundMainSize =
                    boundAxis(currentRelativeChild.contents, mainAxis, baseMainSize);
                  if (baseMainSize != boundMainSize) {
                    /* By excluding this item's size and flex factor from
                     * remaining, this item's min/max constraints should also
                     * trigger in the second pass resulting in the item's size
                     * calculation being identical in the first and second
                     * passes. */
                    deltaFreeSpace.contents =
                      deltaFreeSpace.contents -. (boundMainSize -. childFlexBasis);
                    deltaFlexShrinkScaledFactors.contents =
                      deltaFlexShrinkScaledFactors.contents -. flexShrinkScaledFactor
                  }
                }
              } else if (remainingFreeSpace.contents > zero) {
                let flexGrowFactor = cssGetFlexGrow(currentRelativeChild.contents);
                /* Is this child able to grow? */
                if (flexGrowFactor != zero) {
                  let baseMainSize =
                    childFlexBasis
                    /*
                     * Important to first scale, then divide - to support fixed
                     * point encoding.
                     */
                    +. flexGrowFactor
                    *. remainingFreeSpace.contents
                    /. totalFlexGrowFactors.contents;
                  let boundMainSize =
                    boundAxis(currentRelativeChild.contents, mainAxis, baseMainSize);
                  if (baseMainSize != boundMainSize) {
                    /* By excluding this item's size and flex factor from
                     * remaining, this item's min/max constraints should also
                     * trigger in the second pass resulting in the item's size
                     * calculation being identical in the first and second
                     * passes. */
                    deltaFreeSpace.contents =
                      deltaFreeSpace.contents -. (boundMainSize -. childFlexBasis);
                    deltaFlexGrowFactors.contents = deltaFlexGrowFactors.contents -. flexGrowFactor
                  }
                }
              };
              currentRelativeChild.contents = currentRelativeChild.contents.nextChild
            };
            totalFlexShrinkScaledFactors.contents =
              totalFlexShrinkScaledFactors.contents +. deltaFlexShrinkScaledFactors.contents;
            totalFlexGrowFactors.contents =
              totalFlexGrowFactors.contents +. deltaFlexGrowFactors.contents;
            remainingFreeSpace.contents = remainingFreeSpace.contents +. deltaFreeSpace.contents;

            /*** Second pass: resolve the sizes of the flexible items */
            deltaFreeSpace.contents = zero;
            currentRelativeChild.contents = firstRelativeChild.contents;
            while (currentRelativeChild.contents !== theNullNode) {
              let childFlexBasis = currentRelativeChild.contents.layout.computedFlexBasis;
              let updatedMainSize = {contents: childFlexBasis};
              if (remainingFreeSpace.contents < zero) {
                let flexShrinkScaledFactor =
                  -. cssGetFlexShrink(currentRelativeChild.contents) *. childFlexBasis;
                /* Is this child able to shrink? */
                if (flexShrinkScaledFactor != zero) {
                  let childSize =
                    if (totalFlexShrinkScaledFactors.contents == zero) {
                      childFlexBasis +. flexShrinkScaledFactor
                    } else {
                      childFlexBasis
                      /*
                       * Important to first scale, then divide - to support
                       * fixed point encoding.
                       */
                      +. flexShrinkScaledFactor
                      *. remainingFreeSpace.contents
                      /. totalFlexShrinkScaledFactors.contents
                    };
                  updatedMainSize.contents =
                    boundAxis(currentRelativeChild.contents, mainAxis, childSize)
                }
              } else if (remainingFreeSpace.contents > zero) {
                let flexGrowFactor = cssGetFlexGrow(currentRelativeChild.contents);
                if (flexGrowFactor != zero) {
                  updatedMainSize.contents =
                    boundAxis(
                      currentRelativeChild.contents,
                      mainAxis,
                      childFlexBasis
                      /*
                       * Important to first scale, then divide - to support
                       * fixed point encoding.
                       */
                      +. flexGrowFactor
                      *. remainingFreeSpace.contents
                      /. totalFlexGrowFactors.contents
                    )
                }
              };
              deltaFreeSpace.contents =
                deltaFreeSpace.contents -. (updatedMainSize.contents -. childFlexBasis);
              let childWidth = {contents: zero};
              let childHeight = {contents: zero};
              let childWidthMeasureMode = {contents: Undefined};
              let childHeightMeasureMode = {contents: Undefined};
              if (isMainAxisRow) {
                childWidth.contents =
                  updatedMainSize.contents +. getMarginAxis(currentRelativeChild.contents, Row);
                childWidthMeasureMode.contents = Exactly;
                if (! isUndefined(availableInnerCrossDim)
                    && ! isStyleDimDefined(currentRelativeChild.contents, Column)
                    && heightMeasureMode === Exactly
                    && getAlignItem(node, currentRelativeChild.contents) === AlignStretch) {
                  childHeight.contents = availableInnerCrossDim;
                  childHeightMeasureMode.contents = Exactly
                } else if (! isStyleDimDefined(currentRelativeChild.contents, Column)) {
                  childHeight.contents = availableInnerCrossDim;
                  childHeightMeasureMode.contents = (
                    if (isUndefined(childHeight.contents)) {
                      Undefined
                    } else {
                      AtMost
                    }
                  )
                } else {
                  childHeight.contents =
                    currentRelativeChild.contents.style.height
                    +. getMarginAxis(currentRelativeChild.contents, Column);
                  childHeightMeasureMode.contents = Exactly
                }
              } else {
                childHeight.contents =
                  updatedMainSize.contents +. getMarginAxis(currentRelativeChild.contents, Column);
                childHeightMeasureMode.contents = Exactly;
                if (! isUndefined(availableInnerCrossDim)
                    && ! isStyleDimDefined(currentRelativeChild.contents, Row)
                    && widthMeasureMode === Exactly
                    && getAlignItem(node, currentRelativeChild.contents) === AlignStretch) {
                  childWidth.contents = availableInnerCrossDim;
                  childWidthMeasureMode.contents = Exactly
                } else if (! isStyleDimDefined(currentRelativeChild.contents, Row)) {
                  childWidth.contents = availableInnerCrossDim;
                  childWidthMeasureMode.contents = (
                    if (isUndefined(childWidth.contents)) {
                      Undefined
                    } else {
                      AtMost
                    }
                  )
                } else {
                  childWidth.contents =
                    currentRelativeChild.contents.style.width
                    +. getMarginAxis(currentRelativeChild.contents, Row);
                  childWidthMeasureMode.contents = Exactly
                }
              };
              /* TODO: #aspectRatio */
              /* if (not (isUndefined currentRelativeChild.contents.style.aspectRatio)) { */
              /*   if (isMainAxisRow && childHeightMeasureMode.contents !== Exactly) { */
              /*     childHeight.contents = */
              /*       fmaxf */
              /*         (childWidth.contents *. currentRelativeChild.style.aspectRatio) */
              /*         (getPaddingAndBorderAxis currentRelativeChild.contents Column); */
              /*     childHeightMeasureMode.contents = Exactly */
              /*   } else if ( */
              /*     not isMainAxisRow && childWidthMeasureMode.contents !== Exactly */
              /*   ) { */
              /*     childWidth.contents = */
              /*       fmaxf */
              /*         (childHeight.contents *. currentRelativeChild.contents.style.aspectRatio) */
              /*         (getPaddingAndBorderAxis currentRelativeChild.contents Row); */
              /*     childWidthMeasureMode.contents = Exactly */
              /*   } */
              /* }; */
              childWidth.contents =
                constrainSizeToMaxSizeForMode(
                  currentRelativeChild.contents.style.maxWidth,
                  childWidthMeasureMode.contents,
                  childWidth.contents
                );
              childWidthMeasureMode.contents =
                constrainModeToMaxSizeForMode(
                  currentRelativeChild.contents.style.maxWidth,
                  childWidthMeasureMode.contents
                );
              childHeight.contents =
                constrainSizeToMaxSizeForMode(
                  currentRelativeChild.contents.style.maxHeight,
                  childHeightMeasureMode.contents,
                  childHeight.contents
                );
              childHeightMeasureMode.contents =
                constrainModeToMaxSizeForMode(
                  currentRelativeChild.contents.style.maxHeight,
                  childHeightMeasureMode.contents
                );
              let requiresStretchLayout =
                ! isStyleDimDefined(currentRelativeChild.contents, crossAxis)
                && getAlignItem(node, currentRelativeChild.contents) === AlignStretch;
              /* Recursively call the layout algorithm for this child with the
               * updated main size. */
              let _ =
                layoutNodeInternal(
                  currentRelativeChild.contents,
                  childWidth.contents,
                  childHeight.contents,
                  direction,
                  childWidthMeasureMode.contents,
                  childHeightMeasureMode.contents,
                  performLayout && ! requiresStretchLayout,
                  flexString
                );
              currentRelativeChild.contents = currentRelativeChild.contents.nextChild
            }
          };
          remainingFreeSpace.contents = originalRemainingFreeSpace +. deltaFreeSpace.contents;

          /*** STEP 6: MAIN-AXIS JUSTIFICATION & CROSS-AXIS SIZE DETERMINATION
           * At this point, all the children have their dimensions set in the
           * main axis.  Their dimensions are also set in the cross axis with
           * the exception of items that are aligned "stretch". We need to
           * compute these stretch values and set the final positions. */

          /*** If we are using "at most" rules in the main axis. Calculate the
           * remaining space when constraint by the min size defined for the
           * main axis. */
          if (measureModeMainDim === AtMost && remainingFreeSpace.contents > zero) {
            let minDim = styleMinDimensionForAxis(node, mainAxis);
            if (! isUndefined(minDim) && minDim >= zero) {
              remainingFreeSpace.contents =
                fmaxf(zero, minDim -. (availableInnerMainDim -. remainingFreeSpace.contents))
            } else {
              remainingFreeSpace.contents = zero
            }
          };
          /* No worries, this shouldn't allocate! https://caml.inria.fr/mantis/view.php?id=4800 */
          let (leadingMainDim, betweenMainDim) =
            switch justifyContent {
            | JustifyCenter => (divideScalarByInt(remainingFreeSpace.contents, 2), zero)
            | JustifyFlexEnd => (remainingFreeSpace.contents, zero)
            | JustifySpaceBetween => (
                zero,
                if (itemsOnLine.contents > 1) {
                  divideScalarByInt(
                    fmaxf(remainingFreeSpace.contents, zero),
                    itemsOnLine.contents - 1
                  )
                } else {
                  zero
                }
              )
            | JustifySpaceAround =>
              let betweenMainDim =
                divideScalarByInt(remainingFreeSpace.contents, itemsOnLine.contents);
              (divideScalarByInt(betweenMainDim, 2), betweenMainDim)
            | JustifyFlexStart => (zero, zero)
            };
          let mainDim = {contents: leadingPaddingAndBorderMain +. leadingMainDim};
          let crossDim = {contents: zero};
          for (i in startOfLineIndex.contents to endOfLineIndex.contents - 1) {
            child.contents = node.children[i];
            if (child.contents.style.positionType === Absolute
                && isLeadingPosDefined(child.contents, mainAxis)) {
              if (performLayout) {
                /* In case the child is position absolute and has left/top
                 * being defined, we override the position to whatever the user
                 * said (and margin/border). */
                setLayoutLeadingPositionForAxis(
                  child.contents,
                  mainAxis,
                  getLeadingPosition(child.contents, mainAxis)
                  +. getLeadingBorder(node, mainAxis)
                  +. getLeadingMargin(child.contents, mainAxis)
                )
              }
            } else if (child.contents.style.positionType === Relative) {
              /* Now that we placed the element, we need to update the
               * variables.  We need to do that only for relative elements.
               * Absolute elements do not take part in that phase. */
              if (performLayout) {
                setLayoutLeadingPositionForAxis(
                  child.contents,
                  mainAxis,
                  layoutPosPositionForAxis(child.contents, mainAxis) +. mainDim.contents
                )
              };
              if (canSkipFlex) {
                /* If we skipped the flex step, then we can't rely on the
                 * measuredDims because they weren't computed. This means we
                 * can't call YGNodeDimWithMargin. */
                mainDim.contents =
                  mainDim.contents
                  +. betweenMainDim
                  +. getMarginAxis(child.contents, mainAxis)
                  +. child.contents.layout.computedFlexBasis;
                crossDim.contents = availableInnerCrossDim
              } else {
                /* The main dimension is the sum of all the elements dimension
                 * plus the spacing. */
                mainDim.contents =
                  mainDim.contents +. betweenMainDim +. getDimWithMargin(child.contents, mainAxis);
                /* The cross dimension is the max of the elements dimension
                 * since there can only be one element in that cross dimension.
                 * */
                crossDim.contents =
                  fmaxf(crossDim.contents, getDimWithMargin(child.contents, crossAxis))
              }
            } else if (performLayout) {
              setLayoutLeadingPositionForAxis(
                child.contents,
                mainAxis,
                layoutPosPositionForAxis(child.contents, mainAxis)
                +. getLeadingBorder(child.contents, mainAxis)
                +. leadingMainDim
              )
            }
          };
          mainDim.contents = mainDim.contents +. trailingPaddingAndBorderMain;
          let containerCrossAxis = {contents: availableInnerCrossDim};
          if (measureModeCrossDim === Undefined || measureModeCrossDim === AtMost) {
            /* Compute the cross axis from the max cross dimension of the children. */
            containerCrossAxis.contents =
              boundAxis(node, crossAxis, crossDim.contents +. paddingAndBorderAxisCross)
              -. paddingAndBorderAxisCross;
            if (measureModeCrossDim === AtMost) {
              containerCrossAxis.contents =
                fminf(containerCrossAxis.contents, availableInnerCrossDim)
            }
          };
          /* If there's no flex wrap, the cross dimension is defined by the container. */
          if (! isNodeFlexWrap && measureModeCrossDim === Exactly) {
            crossDim.contents = availableInnerCrossDim
          };
          /* Clamp to the min/max size specified on the container. */
          crossDim.contents =
            boundAxis(node, crossAxis, crossDim.contents +. paddingAndBorderAxisCross)
            -. paddingAndBorderAxisCross;
          /*
           * STEP 7: CROSS-AXIS ALIGNMENT We can skip child alignment if we're
           * just measuring the container.
           */
          if (performLayout) {
            for (i in startOfLineIndex.contents to endOfLineIndex.contents - 1) {
              child.contents = node.children[i];
              if (child.contents.style.positionType === Absolute) {
                /* If the child is absolutely positioned and has a
                 * top/left/bottom/right set, override all the previously
                 * computed positions to set it correctly. */
                if (isLeadingPosDefined(child.contents, crossAxis)) {
                  setLayoutLeadingPositionForAxis(
                    child.contents,
                    crossAxis,
                    getLeadingPosition(child.contents, crossAxis)
                    +. getLeadingBorder(node, crossAxis)
                    +. getLeadingMargin(child.contents, crossAxis)
                  )
                } else {
                  setLayoutLeadingPositionForAxis(
                    child.contents,
                    crossAxis,
                    getLeadingBorder(node, crossAxis)
                    +. getLeadingMargin(child.contents, crossAxis)
                  )
                }
              } else {
                let leadingCrossDim = {contents: leadingPaddingAndBorderCross};
                /* For a relative children, we're either using alignItems
                 * (parent) or alignSelf (child) in order to determine the
                 * position in the cross axis */
                let alignItem = getAlignItem(node, child.contents);
                /* If the child uses align stretch, we need to lay it out one
                 * more time, this time forcing the cross-axis size to be the
                 * computed cross size for the current line. */
                if (alignItem === AlignStretch) {
                  let isCrossSizeDefinite =
                    isMainAxisRow
                    && isStyleDimDefined(child.contents, Column)
                    || ! isMainAxisRow
                    && isStyleDimDefined(child.contents, Row);
                  let childWidthMeasureMode = Exactly;
                  let childHeightMeasureMode = Exactly;
                  let (childHeight, childWidth) =
                    if (isMainAxisRow) {
                      (
                        crossDim.contents,
                        child.contents.layout.measuredWidth +. getMarginAxis(child.contents, Row)
                      )
                    } else {
                      (
                        child.contents.layout.measuredHeight
                        +. getMarginAxis(child.contents, Column),
                        crossDim.contents
                      )
                    };
                  /* Strangely, in Yoga, we don't even use the
                   * `childWidthMeasureMode`/`childHeightMeasureMode`, so it's
                   * a good thing that we ended up breaking the
                   * `YGConstrainMaxSizeForMode` function into two functions
                   * (one for determining dim, the other for determining the
                   * mode) */
                  let childWidth =
                    constrainSizeToMaxSizeForMode(
                      child.contents.style.maxWidth,
                      childWidthMeasureMode,
                      childWidth
                    );
                  let childHeight =
                    constrainSizeToMaxSizeForMode(
                      child.contents.style.maxHeight,
                      childHeightMeasureMode,
                      childHeight
                    );
                  /* If the child defines a definite size for its cross axis,
                   * there's no need to stretch. */
                  if (! isCrossSizeDefinite) {
                    let childWidthMeasureMode =
                      if (isUndefined(childWidth)) {
                        Undefined
                      } else {
                        Exactly
                      };
                    let childHeightMeasureMode =
                      if (isUndefined(childHeight)) {
                        Undefined
                      } else {
                        Exactly
                      };
                    let _ =
                      layoutNodeInternal(
                        child.contents,
                        childWidth,
                        childHeight,
                        direction,
                        childWidthMeasureMode,
                        childHeightMeasureMode,
                        true,
                        stretchString
                      );
                    ()
                  }
                } else if (alignItem !== AlignFlexStart) {
                  let remainingCrossDim =
                    containerCrossAxis.contents -. getDimWithMargin(child.contents, crossAxis);
                  if (alignItem === AlignCenter) {
                    leadingCrossDim.contents =
                      leadingCrossDim.contents +. divideScalarByInt(remainingCrossDim, 2)
                  } else {
                    leadingCrossDim.contents = leadingCrossDim.contents +. remainingCrossDim
                  }
                };
                setLayoutLeadingPositionForAxis(
                  child.contents,
                  crossAxis,
                  layoutPosPositionForAxis(child.contents, crossAxis)
                  +. (totalLineCrossDim.contents +. leadingCrossDim.contents)
                )
              }
            }
          };
          totalLineCrossDim.contents = totalLineCrossDim.contents +. crossDim.contents;
          maxLineMainDim.contents = fmaxf(maxLineMainDim.contents, mainDim.contents);
          lineCount.contents = lineCount.contents + 1;
          startOfLineIndex.contents = endOfLineIndex.contents
        };

        /*** STEP 8: MULTI-LINE CONTENT ALIGNMENT */
        if (lineCount.contents > 1 && performLayout && ! isUndefined(availableInnerCrossDim)) {
          let remainingAlignContentDim = availableInnerCrossDim -. totalLineCrossDim.contents;
          let crossDimLead = {contents: zero};
          let currentLead = {contents: leadingPaddingAndBorderCross};
          let alignContent = node.style.alignContent;
          switch alignContent {
          | AlignFlexEnd => currentLead.contents = currentLead.contents +. remainingAlignContentDim
          | AlignCenter =>
            currentLead.contents =
              currentLead.contents +. divideScalarByInt(remainingAlignContentDim, 2)
          | AlignStretch =>
            if (availableInnerCrossDim > totalLineCrossDim.contents) {
              crossDimLead.contents =
                divideScalarByInt(remainingAlignContentDim, lineCount.contents)
            }
          | AlignAuto => ()
          | AlignFlexStart => ()
          };
          let endIndex = {contents: 0};
          for (i in 0 to lineCount.contents - 1) {
            let startIndex = endIndex.contents;
            let j = {contents: startIndex};
            let lineHeight = {contents: zero};
            let shouldContinue = {contents: false};
            while (j.contents < childCount && shouldContinue.contents) {
              child.contents = node.children[j.contents];
              if (child.contents.style.positionType === Relative) {
                if (child.contents.lineIndex !== i) {
                  shouldContinue.contents = false
                } else if (isLayoutDimDefined(child.contents, crossAxis)) {
                  lineHeight.contents =
                    fmaxf(
                      lineHeight.contents,
                      layoutMeasuredDimensionForAxis(child.contents, crossAxis)
                      +. getMarginAxis(child.contents, crossAxis)
                    );
                  j.contents = j.contents + 1
                } else {
                  j.contents = j.contents + 1
                }
              } else {
                j.contents = j.contents + 1
              }
            };
            endIndex.contents = j.contents;
            lineHeight.contents = lineHeight.contents +. crossDimLead.contents;
            if (performLayout) {
              for (j in startIndex to endIndex.contents - 1) {
                child.contents = node.children[j];
                if (child.contents.style.positionType === Relative) {
                  switch (getAlignItem(node, child.contents)) {
                  | AlignFlexStart =>
                    setLayoutLeadingPositionForAxis(
                      child.contents,
                      crossAxis,
                      currentLead.contents +. getLeadingMargin(child.contents, crossAxis)
                    )
                  | AlignFlexEnd =>
                    setLayoutLeadingPositionForAxis(
                      child.contents,
                      crossAxis,
                      currentLead.contents
                      +. lineHeight.contents
                      -. getTrailingMargin(child.contents, crossAxis)
                      -. layoutMeasuredDimensionForAxis(child.contents, crossAxis)
                    )
                  | AlignCenter =>
                    let childHeight = layoutMeasuredDimensionForAxis(child.contents, crossAxis);
                    setLayoutLeadingPositionForAxis(
                      child.contents,
                      crossAxis,
                      currentLead.contents
                      +. divideScalarByInt(lineHeight.contents -. childHeight, 2)
                    )
                  | AlignStretch =>
                    setLayoutLeadingPositionForAxis(
                      child.contents,
                      crossAxis,
                      currentLead.contents +. getLeadingMargin(child.contents, crossAxis)
                    )
                  | AlignAuto => raise(Invalid_argument("getAlignItem should never return auto"))
                  }
                }
              }
            };
            currentLead.contents = currentLead.contents +. lineHeight.contents
          }
        };
        /* STEP 9: COMPUTING FINAL DIMENSIONS */
        node.layout.measuredWidth = boundAxis(node, Row, availableWidth -. marginAxisRow);
        node.layout.measuredHeight = boundAxis(node, Column, availableHeight -. marginAxisColumn);
        /* If the user didn't specify a width or height for the node, set the
         * dimensions based on the children. */
        if (measureModeMainDim === Undefined) {
          setLayoutMeasuredDimensionForAxis(
            node,
            mainAxis,
            boundAxis(node, mainAxis, maxLineMainDim.contents)
          )
        } else if (measureModeMainDim === AtMost) {
          setLayoutMeasuredDimensionForAxis(
            node,
            mainAxis,
            fmaxf(
              fminf(
                availableInnerMainDim +. paddingAndBorderAxisMain,
                boundAxisWithinMinAndMax(node, mainAxis, maxLineMainDim.contents)
              ),
              paddingAndBorderAxisMain
            )
          )
        };
        if (measureModeCrossDim === Undefined) {
          setLayoutMeasuredDimensionForAxis(
            node,
            crossAxis,
            boundAxis(node, crossAxis, totalLineCrossDim.contents +. paddingAndBorderAxisCross)
          )
        } else if (measureModeCrossDim === AtMost) {
          setLayoutMeasuredDimensionForAxis(
            node,
            crossAxis,
            fmaxf(
              fminf(
                availableInnerCrossDim +. paddingAndBorderAxisCross,
                boundAxisWithinMinAndMax(
                  node,
                  crossAxis,
                  totalLineCrossDim.contents +. paddingAndBorderAxisCross
                )
              ),
              paddingAndBorderAxisCross
            )
          )
        };
        if (performLayout) {
          /* STEP 10: SIZING AND POSITIONING ABSOLUTE CHILDREN */
          let currentAbsoluteChildRef = {contents: firstAbsoluteChild.contents};
          while (currentAbsoluteChildRef.contents !== theNullNode) {
            absoluteLayoutChild(
              node,
              currentAbsoluteChildRef.contents,
              availableInnerWidth,
              widthMeasureMode,
              direction
            );
            currentAbsoluteChildRef.contents = currentAbsoluteChildRef.contents.nextChild
          };
          /* STEP 11: SETTING TRAILING POSITIONS FOR CHILDREN */
          let needsMainTrailingPos = mainAxis == RowReverse || mainAxis == ColumnReverse;
          let needsCrossTrailingPos = crossAxis == RowReverse || crossAxis == ColumnReverse;
          /* Set trailing position if necessary. */
          if (needsMainTrailingPos || needsCrossTrailingPos) {
            for (i in 0 to childCount - 1) {
              let child = node.children[i];
              if (needsMainTrailingPos) {
                setTrailingPosition(node, child, mainAxis)
              };
              if (needsCrossTrailingPos) {
                setTrailingPosition(node, child, crossAxis)
              }
            }
          }
        }
      }
    }
  };
  let layoutNode = (node, availableWidth, availableHeight, parentDirection) => {
    /* Increment the generation count. This will force the recursive routine to visit*/
    /* all dirty nodes at least once. Subsequent visits will be skipped if the input*/
    /* parameters don't change.*/
    gCurrentGenerationCount.contents = gCurrentGenerationCount.contents + 1;
    /* If the caller didn't specify a height/width, use the dimensions*/
    /* specified in the style.*/
    let (width, widthMeasureMode) =
      if (! isUndefined(availableWidth)) {
        (availableWidth, Exactly)
      } else if (isStyleDimDefined(node, Row)) {
        (node.style.width +. getMarginAxis(node, Row), Exactly)
      } else if
        /* TODO: #correctness: This should guard for undefined case. */
        (node.style.maxWidth >= zero) {
        (node.style.maxWidth, AtMost)
      } else {
        (availableWidth, Undefined)
      };
    let (height, heightMeasureMode) =
      if (! isUndefined(availableHeight)) {
        (availableHeight, Exactly)
      } else if (isStyleDimDefined(node, Column)) {
        (node.style.height +. getMarginAxis(node, Column), Exactly)
      } else if
        /* TODO: #correctness: This should guard for undefined case. */
        (node.style.maxHeight >= zero) {
        (node.style.maxHeight, AtMost)
      } else {
        (availableHeight, Undefined)
      };
    if (layoutNodeInternal(
          node,
          width,
          height,
          parentDirection,
          widthMeasureMode,
          heightMeasureMode,
          true,
          initialString
        )) {
      setPosition(node, node.layout.direction);
      if (gPrintTree.contents) {
        LayoutPrint.printCssNode((
          node,
          {printLayout: true, printChildren: true, printStyle: true}
        ))
      }
    }
  };
};
