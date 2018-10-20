open Cocoa;

module Node = {
  type context = NSView.t;
  let nullContext = NSView.make();
};

module Style = {
  type color = [
    | `rgb(int, int, int)
    | `rgba(int, int, int, float)
    | `hsl(int, int, int)
    | `hsla(int, int, int, float)
    | `hex(string)
    | `transparent
  ];

  type width = [ | `pt(float) | `pct(float)];

  let convertColor: color => (float, float, float, float) =
    fun
    | `rgb(r, g, b) => (
        float_of_int(r),
        float_of_int(g),
        float_of_int(b),
        1.,
      )
    | `rgba(r, g, b, a) => (
        float_of_int(r),
        float_of_int(g),
        float_of_int(b),
        a,
      )
    | _ => failwith("Other colors not implemented yet");

  let convertWidth: width => float =
    fun
    | `pt(w) => w
    | _ => failwith("Other width types not implemented yet");
};

include Flex.Layout.Create(Node, Flex.FloatEncoding);

let makeLayoutNode = (~layout, hostView) =>
  LayoutSupport.createNode(~withChildren=[||], ~andStyle=layout, hostView);

let cssNodeInsertChild = (node, child, index) => {
  open LayoutSupport;
  open LayoutTypes;
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

  LayoutSupport.markDirtyInternal(node);
};
