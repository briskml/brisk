open Brisk_macos;

module OutputTree = {
  type node = {
    toolbarItems: list(Toolbar.Reconciler.syntheticElement),
    contentViewElement: Brisk.syntheticElement,
    children: list(node),
  };

  let rec getToolbarItems = node => {
    switch (node.children) {
    | [] => node.toolbarItems
    | l => List.map(getToolbarItems, l) |> List.concat
    };
  };

  let rec getContentViewElement = node => {
    switch (node.children) {
    | [] =>
      node.contentViewElement;
    | l =>
      List.map(getContentViewElement, l) |> Brisk.listToElement;
    };
  };

  let markAsStale = Brisk.OutputTree.markAsStale;

  let insert = (elem, index, l) => {
    let rec acc = (currentindex, tail) =>
      switch (tail) {
      | t when index === currentindex => [elem, ...t]
      | [h, ...t] => [h, ...acc(currentindex + 1, t)]
      | [] =>
        raise(Invalid_argument("index is bigger than the length of list"))
      };
    acc(0, l);
  };

  let delete = (index, l) => {
    let rec acc = (currentIndex, tail) =>
      switch (tail) {
      | [] =>
        raise(Invalid_argument("index is bigger than the length of list"))
      | [_, ...t] when currentIndex === index => t
      | [h, ...t] => [h, ...acc(currentIndex + 1, t)]
      };
    acc(0, l);
  };

  let insertNode = (~parent: node, ~child: node, ~position: int) => {
    {...parent, children: insert(child, position, parent.children)};
  };

  let deleteNode = (~parent: node, ~child as _, ~position) => {
    {...parent, children: delete(position, parent.children)};
  };

  let moveNode = (~parent, ~child, ~from, ~to_) => {
    let parent = deleteNode(~parent, ~child, ~position=from);
    insertNode(~parent, ~child, ~position=to_);
  };
};

module Reconciler = Brisk_reconciler.Make(OutputTree);

let component = Reconciler.nativeComponent("navigation.screen");
let screen =
    (
      ~children: list(Reconciler.syntheticElement),
      ~toolbarItems: list(Toolbar.Reconciler.syntheticElement),
      ~contentView: Brisk.syntheticElement,
      (),
    ) =>
  component(hooks =>
    (
      hooks,
      {
        make: () => {
          toolbarItems,
          contentViewElement: contentView,
          children: [],
        },
        configureInstance: (~isFirstRender as _, node) => {
          {
            ...node,
            toolbarItems,
            contentViewElement: contentView,
          };
        },
        children: Reconciler.listToElement(children),
      },
    )
  );
