include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "./test/snapshots",
      projectDir: "./test",
    });
});
