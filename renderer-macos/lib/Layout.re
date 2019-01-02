open Cocoa;

module Node = {
  type context = NSView.t;
  let nullContext = NSView.make();
};

include Brisk_renderer.CreateLayout(Node, Flex.FloatEncoding);
