module Node = {
  type context = BriskView.t;
  let nullContext = BriskView.make();
};

include Brisk_renderer.CreateLayout(Node, Flex.FloatEncoding);
