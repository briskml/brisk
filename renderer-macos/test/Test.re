open TestFramework;

describe("Test simple expression", ({test}) => {
  test("1 + 1 should equal 2", ({expect}) =>
    expect.int(1 + 1).toBe(2)
  );

  test("1 == 1 should be true", ({expect}) =>
    expect.bool(1 == 1).toBe(true)
  );
});

let () = cli();
