module Create = (Node: Spec.Node, Encoding: Spec.Encoding) => {
  type printOptions = {
    printLayout: bool,
    printStyle: bool,
    printChildren: bool
  };
  type edge =
    | Left
    | Top
    | Right
    | Bottom
    | Start
    | End
    | Horizontal
    | Vertical
    | All;
  type direction =
    | CSS_DIRECTION_NEGATIVE_ONE_WHATEVER_THAT_MEANS /* inherit */
    | Inherit /* 'inherit' */
    | Ltr /* 'ltr'     */
    | Rtl /* 'rtl'     */;
  type flexDirection =
    /* 'row-reverse' */
    | Column /* 'column' */
    | ColumnReverse
    | Row /* 'row' */
    | RowReverse;
  /* 'column-reverse' */
  type justify =
    | JustifyFlexStart /* 'flex-start' */
    | JustifyCenter /* 'center' */
    | JustifyFlexEnd /* 'flex-end' */
    | JustifySpaceBetween /* 'space-between' */
    | JustifySpaceAround /* space-around' */;
  type align =
    | AlignAuto
    | AlignFlexStart /* 'flex-start' */
    | AlignCenter /* 'center' */
    | AlignFlexEnd /* 'flex-end' */
    | AlignStretch /* 'stretch' */;
  type positionType =
    | Relative /* 'relative' */
    | Absolute /* 'absolute' */;
  type measureMode =
    /* there are some places in the reference implementation that set
     * the measure mode to negative one.*/
    /* https://github.com/facebook/css-layout/pull/185#r64995699 */
    | Undefined /* 'undefined' */
    | Exactly /* 'exactly' */
    | AtMost /* 'at-most' */
    | CSS_MEASURE_MODE_NEGATIVE_ONE_WHATEVER_THAT_MEANS;
  let css_max_cached_result_count = 6;

  /***
   * We really *want* to be using the definitions from `Encoding`, but we want
   * to guarantee that all of the floating point operations are inlined.
   * Therefore, we directly reference the `HardCodedEncoding`. functors,
   */
  type unitOfM = HardCodedEncoding.scalar;

  /***
   * Intentionally, nothing is mutable inside each
   */
  type cachedMeasurement = {
    mutable availableWidth: unitOfM,
    mutable availableHeight: unitOfM,
    mutable widthMeasureMode: measureMode,
    mutable heightMeasureMode: measureMode,
    mutable computedWidth: unitOfM,
    mutable computedHeight: unitOfM
  };
  type overflow =
    | Visible
    | Hidden
    | Scroll;
  type wrapType =
    | CssNoWrap
    | CssWrap;
  type dimensions = {
    width: unitOfM,
    height: unitOfM
  };
  type coordinates = {
    left: unitOfM,
    top: unitOfM
  };
  /* TODO: Benchmark the immutable version versus mutable version */
  type cssStyle = {
    mutable direction,
    mutable flexDirection,
    mutable justifyContent: justify,
    mutable alignContent: align,
    mutable alignItems: align,
    mutable alignSelf: align,
    mutable positionType,
    mutable flexWrap: wrapType,
    mutable overflow,
    mutable flex: unitOfM,
    mutable flexGrow: unitOfM,
    mutable flexShrink: unitOfM,
    mutable flexBasis: unitOfM,
    mutable marginLeft: unitOfM,
    mutable marginTop: unitOfM,
    mutable marginRight: unitOfM,
    mutable marginBottom: unitOfM,
    mutable marginStart: unitOfM,
    mutable marginEnd: unitOfM,
    mutable marginHorizontal: unitOfM,
    mutable marginVertical: unitOfM,
    mutable margin: unitOfM,
    mutable width: unitOfM,
    mutable height: unitOfM,
    mutable minWidth: unitOfM,
    mutable minHeight: unitOfM,
    mutable maxWidth: unitOfM,
    mutable maxHeight: unitOfM,
    mutable left: unitOfM,
    mutable top: unitOfM,
    mutable right: unitOfM,
    mutable bottom: unitOfM,
    /***
     * Start position.
     */
    mutable start: unitOfM,
    /***
     * End position.
     */
    mutable endd: unitOfM,
    mutable horizontal: unitOfM,
    mutable vertical: unitOfM,
    mutable position: unitOfM,
    /***
     * You should skip all the rules that contain negative values for the
     * following attributes. For example:
     *   {padding: 10, paddingLeft: -5}
     * should output:
     *   {left: 10 ...}
     * the following two are incorrect:
     *   {left: -5 ...}
     *   {left: 0 ...}
     */
    mutable paddingLeft: unitOfM,
    mutable paddingTop: unitOfM,
    mutable paddingRight: unitOfM,
    mutable paddingBottom: unitOfM,
    mutable paddingStart: unitOfM,
    mutable paddingEnd: unitOfM,
    mutable paddingHorizontal: unitOfM,
    mutable paddingVertical: unitOfM,
    mutable padding: unitOfM,
    mutable borderLeft: unitOfM,
    mutable borderTop: unitOfM,
    mutable borderRight: unitOfM,
    mutable borderBottom: unitOfM,
    mutable borderStart: unitOfM,
    mutable borderEnd: unitOfM,
    mutable borderHorizontal: unitOfM,
    mutable borderVertical: unitOfM,
    mutable border: unitOfM
  };

  /***
   * Analog to "computed style" - the position takes into account all of the CSS
   * styles and inheritance.
   */
  type cssLayout = {
    mutable left: unitOfM,
    mutable top: unitOfM,
    mutable right: unitOfM,
    mutable bottom: unitOfM,
    mutable width: unitOfM,
    mutable height: unitOfM,
    mutable direction,
    /* Instead of recomputing the entire layout every single time, we
     * cache some information to break early when nothing changed */
    mutable generationCount: int,
    mutable computedFlexBasisGeneration: int,
    mutable lastParentDirection: direction,
    mutable computedFlexBasis: unitOfM,
    mutable nextCachedMeasurementsIndex: int,
    /***
     * Hardcoded to 6 previous measurements.
     */
    mutable cachedMeasurement1: cachedMeasurement,
    mutable cachedMeasurement2: cachedMeasurement,
    mutable cachedMeasurement3: cachedMeasurement,
    mutable cachedMeasurement4: cachedMeasurement,
    mutable cachedMeasurement5: cachedMeasurement,
    mutable cachedMeasurement6: cachedMeasurement,
    mutable measuredWidth: unitOfM,
    mutable measuredHeight: unitOfM,
    mutable cachedLayout: cachedMeasurement
  };
  type node = {
    mutable selfRef: nativeint, /* a C pointer pointing to itself, needed for introp with C's memory management */
    mutable style: cssStyle,
    layout: cssLayout,
    mutable lineIndex: int,
    mutable parent: node,
    mutable nextChild: node,
    mutable hasNewLayout: bool,
    mutable measure: option(((node, unitOfM, measureMode, unitOfM, measureMode) => dimensions)),
    print: option((Node.context => unit)),
    mutable children: array(node),
    mutable childrenCount: int,
    mutable isDirty: bool,
    mutable context: Node.context
  };
};
