module Node = {
  type context = BriskView.t;
  let nullContext = BriskView.make();
};

include Brisk_core.CreateLayout(Node, Flex.FloatEncoding);
