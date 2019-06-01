type loadingState('a) =
  | Loading
  | Error(GraphQLClient.Error.t, list(list('a)))
  | Ok(list(list('a)));

let getResultList =
  fun
  | Loading => []
  | Error(_, l) => l
  | Ok(l) => l;

module Map = Core_kernel.Map;
let emptyResourceMap = Core_kernel.Map.empty((module Core_kernel.String));

let hook = (makeRequest, resource, pageSize, hooks) => {
  open Brisk_macos.Brisk;
  let (cache, updateCache, hooks) = Hooks.state(emptyResourceMap, hooks);
  let cachedData = Map.find(cache, resource);
  let (loadNextPageAction, _, hooks) =
    Hooks.state(RemoteAction.create(), hooks);
  let (offset, results) =
    switch (cachedData) {
    | Some((offset, results)) => (offset, results)
    | None => (0, Loading)
    };
  let loadNext = () => {
    makeRequest(~offset, ~pageSize)
    |> Lwt.map(
         fun
         | Result.Ok(next) => {
             updateCache(
               Map.set(
                 cache,
                 ~key=resource,
                 ~data=(
                   offset + pageSize,
                   Ok([next, ...getResultList(results)]),
                 ),
               ),
             );
           }
         | Result.Error(err) => {
             updateCache(
               Map.set(
                 cache,
                 ~key=resource,
                 ~data=(
                   offset + pageSize,
                   Error(err, getResultList(results)),
                 ),
               ),
             );
           },
       )
    |> ignore;
  };
  let hooks =
    Hooks.effect(
      OnMount,
      () => {
        loadNext();
        None;
      },
      hooks,
    );
  let hooks =
    Hooks.effect(
      If((!=), resource),
      () => {
        loadNext();
        None;
      },
      hooks,
    );
  let hooks =
    Hooks.effect(
      Always,
      () =>
        Some(RemoteAction.subscribe(~handler=loadNext, loadNextPageAction)),
      hooks,
    );
  (results, () => RemoteAction.send(~action=(), loadNextPageAction), hooks);
};
