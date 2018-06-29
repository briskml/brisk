open Brisk_cocoa;

/* let otherComponent = React.reducerComponent("Other"); */

/* external registerLoop : (unit => unit) => unit = "CA_registerLoop"; */

module View = {
  let createElement = (~layout, ~style, ~borderColor, ~children, ()) =>
    React.(
      element(
        View.make(~layout, ~style, ~borderColor, listToElement(children)),
      )
    );
};

module Button = {
  let createElement = (~style, ~text, ~callback=?, ~children, ()) =>
    React.(
      element(
        Button.make(~text, ~style, ~callback?, listToElement(children)),
      )
    );
};

/* let performLayout = (window, outputTree) => {
     React.Layout.(
       layoutNode(
         window.React.HostImplementation.layoutNode,
         Flex.FixedEncoding.cssUndefined,
         Flex.FixedEncoding.cssUndefined,
         Ltr,
       )
     );
     ignore(
       React.NativeView.setFrame(
         window.layoutNode.layout.left,
         window.layoutNode.layout.top,
         window.layoutNode.layout.width,
         window.layoutNode.layout.height,
         window.view,
       ),
     );
     React.OutputTree.traverseHostViewTree(
       ({view, layoutNode}) =>
         ignore(
           React.NativeView.setFrame(
             layoutNode.layout.left,
             layoutNode.layout.top,
             layoutNode.layout.width,
             layoutNode.layout.height,
             view,
           ),
         ),
       outputTree,
     );
   }; */

let render = element => (); /* let window = {
         React.HostImplementation.view: React.NativeView.getWindow(),
         layoutNode:
           React.Layout.LayoutSupport.createNode(
             ~withChildren=[||],
             ~andStyle={
               ...React.Layout.LayoutSupport.defaultStyle,
               width: 320,
               height: 480,
             },
             (),
           ),
       };
       let rendered = ref(React.RenderedElement.render(React.element(element))); */

/* let outputTree = React.OutputTree.fromRenderedElement(window, rendered^);
   React.OutputTree.mountForest(~forest=outputTree, ~nearestParentView=window);
   registerLoop(() => {
     let (newRendered, updateLog) =
       React.RenderedElement.flushPendingUpdates(rendered^);
     rendered := newRendered;
     React.OutputTree.applyUpdateLog(outputTree, updateLog);
     performLayout(window, outputTree);
   }); */

/* module Component = {
     let createElement = (~children, ()) => {
       ...otherComponent,
       initialState: _ => false,
       reducer: (x, _) => React.Update(x),
       render: ({state, reduce}) =>
         if (state) {
           <View
             layout={
               ...React.Layout.LayoutSupport.defaultStyle,
               width: 100,
               height: 100,
             }
             style={
               borderWidth: 1.,
               backgroundColor: {
                 red: 0.3,
                 green: 0.5,
                 blue: 0.3,
                 alpha: 1.,
               },
             }
             borderColor={red: 0., green: 1., blue: 0., alpha: 1.}>
             <Button
               style=React.Layout.LayoutSupport.{
                       ...defaultStyle,
                       width: 100,
                       height: 100,
                     }
               text="Cell one"
               callback=(reduce(() => ! state))
             />
             <Button
               style=React.Layout.LayoutSupport.{
                       ...defaultStyle,
                       width: 100,
                       height: 100,
                     }
               text="Cell two"
               callback=(reduce(() => ! state))
             />
           </View>;
         } else {
           <View
             layout=React.Layout.LayoutSupport.{
                      ...defaultStyle,
                      width: 100,
                      height: 100,
                      paddingTop: 80,
                      paddingBottom: 80,
                    }
             style={
               borderWidth: 1.,
               backgroundColor: {
                 red: 0.3,
                 green: 0.2,
                 blue: 0.1,
                 alpha: 1.,
               },
             }
             borderColor={red: 0., green: 0., blue: 1., alpha: 1.}>
             <Button
               style=React.Layout.LayoutSupport.{
                       ...defaultStyle,
                       width: 100,
                       height: 100,
                     }
               text="well"
               callback=(reduce(() => ! state))
             />
           </View>;
         },
     };
   };


   render(<Component />); */
