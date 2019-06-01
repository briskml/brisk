open Brisk_macos;
module Impl = {
  module BriskRE = UIRunner;
  module ToolbarRE = Toolbar.Reconciler.RenderedElement;
  module NavigationRE = Navigation.Reconciler.RenderedElement;

  type root = {
    window: Brisk_macos__CocoaTypes.window,
    contentView: Brisk_macos__CocoaTypes.view,
  };

  type outputNode = Navigation.OutputTree.node;

  type renderedElement = {
    toolbar: ToolbarRE.t,
    previousToolbarElement: Toolbar.Reconciler.syntheticElement,
    content: BriskRE.renderedElement,
    previousContentElement: Brisk.syntheticElement,
    navigation: NavigationRE.t,
    window: Brisk_macos__CocoaTypes.window,
  };

  type syntheticElement = Navigation.Reconciler.syntheticElement;

  let isDirty = UIRunner.isDirty;

  let setDirty = UIRunner.setDirty;

  let render = ({window, contentView: view}, element) => {
    let navigationRoot =
      Navigation.OutputTree.{
        contentViewElement: Brisk.empty,
        children: [],
        toolbarItems: [],
      };
    open Brisk.Layout;

    let layoutNode =
      Node.make(
        ~style=[
          width(Cocoa.Window.contentWidth(window)),
          height(Cocoa.Window.contentHeight(window)),
        ],
        {view, isYAxisFlipped: false},
      );

    let navigation = NavigationRE.render(navigationRoot, element);
    let node = NavigationRE.executeHostViewUpdates(navigation);
    let toolbarItems = Navigation.OutputTree.getToolbarItems(node);
    let contentViewElement =
      Navigation.OutputTree.getContentViewElement(node);
    let previousToolbarElement = <toolbar> ...toolbarItems </toolbar>;
    let toolbar = ToolbarRE.render(Window(window), previousToolbarElement);
    let contentView =
      BriskRE.render(
        {Brisk.OutputTree.view, layoutNode},
        contentViewElement,
      );
    {
      toolbar,
      content: contentView,
      navigation,
      previousToolbarElement,
      previousContentElement: contentViewElement,
      window,
    };
  };

  let flushPendingUpdates =
      (
        {
          toolbar: toolbarRenderedElement,
          content,
          navigation,
          previousToolbarElement,
          previousContentElement,
          window,
        },
      ) => {
    let navigation = NavigationRE.flushPendingUpdates(navigation);
    let node = NavigationRE.executeHostViewUpdates(navigation);
    let toolbarItems = Navigation.OutputTree.getToolbarItems(node);
    let contentViewElement =
      Navigation.OutputTree.getContentViewElement(node);
    let toolbarElement = <toolbar> ...toolbarItems </toolbar>;
    {
      window,
      content:
        BriskRE.update(
          ~previousElement=previousContentElement,
          ~renderedElement=BriskRE.flushPendingUpdates(content),
          contentViewElement,
        ),
      previousContentElement: contentViewElement,
      toolbar:
        ToolbarRE.update(
          ~previousElement=previousToolbarElement,
          ~renderedElement=
            ToolbarRE.flushPendingUpdates(toolbarRenderedElement),
          toolbarElement,
        ),
      navigation,
      previousToolbarElement: toolbarElement,
    };
  };

  let executePendingEffects =
      ({toolbar, content, navigation, _} as renderedElement) => {
    let navigation = NavigationRE.executePendingEffects(navigation);
    let toolbar = ToolbarRE.executePendingEffects(toolbar);
    let content = BriskRE.executePendingEffects(content);
    {...renderedElement, content, toolbar, navigation};
  };

  let executeHostViewUpdates = ({content, navigation, toolbar, window, _}) => {
    ToolbarRE.executeHostViewUpdates(toolbar) |> ignore;
    /* Toolbar related operations might have changed the window size */
    BriskRE.setWindowHeight(Cocoa.Window.contentHeight(window));
    BriskRE.setWindowWidth(Cocoa.Window.contentWidth(window));
    BriskRE.executeHostViewUpdates(content) |> ignore;
    NavigationRE.executeHostViewUpdates(navigation);
  };
};

include RunLoop.Make(Impl);
