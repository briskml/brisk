module type Node = {type context; let nullContext: context;};

module type Encoding = {

  /***
   * Custom encoding of layout related information. This is ignored, and instead
   */
  type scalar;
  let cssUndefined: scalar;
  let isUndefined: scalar => bool;
  let isDefined: scalar => bool;
  let zero: scalar;
  let negativeOne: scalar;
  let divideScalarByInt: (scalar, int) => scalar;
  let scalarToFloat: scalar => float;
  let (-.): (scalar, scalar) => scalar;
  let (+.): (scalar, scalar) => scalar;
  let (/.): (scalar, scalar) => scalar;
  let ( *. ): (scalar, scalar) => scalar;
  let (~-.): scalar => scalar;
  let scalarToString: scalar => string;
};
