type scalar = int;

let cssUndefined = min_int;

let isUndefined = (num) => num == cssUndefined;

let isDefined = (num) => num != cssUndefined;

let zero = 0;

let negativeOne = (-1);

let divideScalarByInt = (/);

let unitsPerPixel = 100.0;

let scalarToFloat = (f: int) => float_of_int(f) /. unitsPerPixel;

let (-.) = (a, b) => a == cssUndefined || b == cssUndefined ? cssUndefined : a - b;

let (+.) = (a, b) => a == cssUndefined || b == cssUndefined ? cssUndefined : a + b;

let (/.) = (a, b) => a == cssUndefined || b == cssUndefined ? cssUndefined : a / b;

let ( *. ) = (a, b) => a == cssUndefined || b == cssUndefined ? cssUndefined : a * b;

let (~-.) = (a) => a == cssUndefined ? cssUndefined : - a;

let scalarToString = string_of_int;
