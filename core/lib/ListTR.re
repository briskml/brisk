let useTailRecursion = l =>
  switch (l) {
  | [_, _, _, _, _, _, _, _, _, _, ..._] => true
  | _ => false
  };
let concat = list => {
  let rec aux = (acc, l) =>
    switch (l) {
    | [] => List.rev(acc)
    | [x, ...rest] => aux(List.rev_append(x, acc), rest)
    };
  useTailRecursion(list) ? aux([], list) : List.concat(list);
};
let map = (f, list) =>
  useTailRecursion(list) ?
    List.rev_map(f, List.rev(list)) : List.map(f, list);
let map3 = (f, list1, list2, list3) => {
  let rec aux = acc =>
    fun
    | ([], [], []) => acc
    | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) =>
      aux([f((x1, x2, x3)), ...acc], (nextList1, nextList2, nextList3))
    | _ => assert(false);
  aux([], (List.rev(list1), List.rev(list2), List.rev(list3)));
};
let fold3 = (f, list1, list2, list3, initialValue) => {
  let rec aux = acc =>
    fun
    | ([], [], []) => acc
    | ([x1, ...nextList1], [x2, ...nextList2], [x3, ...nextList3]) => {
        let nextRes = f(acc, x1, x2, x3);
        aux(nextRes, (nextList1, nextList2, nextList3));
      }
    | _ => assert(false);
  aux(initialValue, (list1, list2, list3));
};
