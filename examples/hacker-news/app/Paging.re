type loadingState('a) =
  | Loading
  | Error(GraphQLClient.Error.t, list(list('a)))
  | Ok(list(list('a)));

let getResultList =
  fun
  | Loading => []
  | Error(_, l) => l
  | Ok(l) => l;

let hook = (makeRequest, resource, pageSize, hooks) => {
  open Brisk_macos.Brisk;
  let (currentResource, setResource, hooks) = Hooks.ref(resource, hooks);
  let (loadNextPageAction, _, hooks) =
    Hooks.state(RemoteAction.create(), hooks);
  let (offset, setOffset, hooks) = Hooks.state(0, hooks);
  let (results, setResults, hooks) = Hooks.state(Loading, hooks);
  let (offset, results) =
    resource === currentResource ? (offset, results) : (0, Loading);
  let loadNext = () => {
    makeRequest(~offset, ~pageSize)
    |> Lwt.map(
         fun
         | Result.Ok(next) => {
             setResource(resource);
             setResults(Ok([next, ...getResultList(results)]));
             setOffset(offset + pageSize);
           }
         | Result.Error(err) => {
             setResource(resource);
             setResults(Error(err, getResultList(results)));
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
