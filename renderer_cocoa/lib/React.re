open Brisk_core;
open Cocoa;

module Node = {
  type context = unit;
  let nullContext = ();
};

module Layout = Flex.Layout.Create(Node, Flex.FloatEncoding);

let makeLayoutNode = (~layout) =>
  Layout.LayoutSupport.createNode(~withChildren=[||], ~andStyle=layout, ());

module NativeCocoa = {
  [@deriving (show({with_path: false}), eq)]
  type hostElement = nsView;

  [@deriving (show({with_path: false}), eq)]
  type hostView = {
    view: hostElement,
    layoutNode: Layout.LayoutSupport.LayoutTypes.node,
  };

  let cssNodeInsertChild = (node, child, index) => {
    open Layout.LayoutSupport;
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
      node.children[i] = node.children[(i - 1)];
    };
    node.childrenCount = node.childrenCount + 1;
    node.children[index] = child;
    child.parent = node;
    Layout.LayoutSupport.markDirtyInternal(node);
  };

  let instanceMap: Hashtbl.t(int, hostView) = Hashtbl.create(1000);

  let getInstance: int => option(hostView) =
    id =>
      Hashtbl.(mem(instanceMap, id) ? Some(find(instanceMap, id)) : None);

  let memoizeInstance = (id, instance) => {
    print_endline("mem hashtbl: " ++ string_of_int(id));
    Hashtbl.add(instanceMap, id, instance);
    NSView.memoize(id, instance.view);
  };

  let freeInstance = id => {
    Hashtbl.remove(instanceMap, id);
    NSView.free(id);
  };

  let markAsDirty = () => ();

  let beginChanges = () => ();

  let commitChanges = () => ();

  let mountChild = (~parent: hostView, ~child: hostView, ~position: int) => {
    print_endline("mount child at" ++ string_of_int(position));
    cssNodeInsertChild(parent.layoutNode, child.layoutNode, position);
    NSView.addSubview(parent.view, child.view, position);
  };

  let unmountChild = (~parent: hostView, ~child: hostView) => ();

  let remountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    ();
};

let run = render => Callback.register("React.run", render);

include ReactCore.Make(NativeCocoa);
