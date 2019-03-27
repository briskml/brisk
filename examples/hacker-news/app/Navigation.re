module OutputTree = {
  type node = Brisk_macos.Brisk.syntheticElement;

  let markAsStale = () => ();

};

module Reconciler = Brisk_reconciler.Make(OutputTree);
