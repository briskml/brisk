/* type scalar = int; */
/* let cssUndefined = min_int; */
/* let isUndefined num => num === cssUndefined; */
/* let isDefined num => num != cssUndefined; */
/* let zero = 0; */
/* let negativeOne = (-1); */
/* let divideScalarByInt s i => { */
/*   if (s === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (i === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   (/) s i; */
/* }; */
/* let unitsPerPixel = 100.0; */
/* let scalarToFloat (f: int) => float_of_int f /. unitsPerPixel; */
/* let (-.) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a - b */
/* }; */
/* let (+.) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a + b */
/* }; */
/* let (/.) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a / b */
/* }; */
/* let ( *. ) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a * b */
/* }; */
/* let (<) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a < b */
/* }; */
/* let (<=) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a <= b */
/* }; */
/* let (>) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a > b */
/* }; */
/* let (>=) a b => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   if (b === cssUndefined) { */
/*     raise (Invalid_argument "RHS is undefined") */
/*   }; */
/*   a >= b */
/* }; */
/* let (~-.) a => { */
/*   if (a === cssUndefined) { */
/*     raise (Invalid_argument "LHS is undefined") */
/*   }; */
/*   - a */
/* }; */
/* let scalarToString = string_of_int; */

/***
 * Portion of the typical `Spec` that is ignored and instead hard coded to the
 * structure items below, so that we can ensure that inlining occurs. Once we
 * upgrade to `4.03+`, this hard coded module that the implementation relies
 * on can be removed, and we can rely on the module factories to produce code
 * as performant as monomorphic.
 */
type scalar = int;

let cssUndefined = min_int;

let isUndefined = (num) => num === cssUndefined;

let isDefined = (num) => num !== cssUndefined;

let zero = 0;

let negativeOne = (-1);

let divideScalarByInt = (/);

let unitsPerPixel = 100.0;

let scalarToFloat = (f: int) => float_of_int(f) /. unitsPerPixel;


/***
 * TODO: Test if it's the ternary that's causing the performance.
 */
let (-.) = (a, b) =>
  if (a == cssUndefined || b == cssUndefined) {
    cssUndefined
  } else {
    a - b
  };

let (+.) = (a, b) =>
  if (a == cssUndefined || b == cssUndefined) {
    cssUndefined
  } else {
    a + b
  };

let (/.) = (/);

let ( *. ) = ( * );

let (~-.) = (~-);

let scalarToString = string_of_int;
/* Comment out the above, and uncomment out below to enable floating point */
/* type scalar = float; */
/* let cssUndefined = nan; */
/* let isUndefined (num : float) => num != num; */
/* let zero = 0.0; */
/* let divideScalarByInt (f : float) (i : int) => f /. float_of_int i; */
/* let (-.) = (-.); */
/* let (+.) = (+.); */
/* let (/.) = (/.); */
/* let ( *. ) = ( *. ); */
/* let (~-.) = (~-.); */
/* let scalarToString = string_of_float; */
/* let scalarToFloat (f: float) => f; */
