#import "BriskCocoa.h"
#import <caml/threads.h>
#import <caml/unixsupport.h>

void ml_printIsMain() { printf("isMainThread: %i\n", [NSThread isMainThread]); }

void ml_dispatchAsyncBackground(value taskFn_v) {
  CAMLparam1(taskFn_v);
  value taskFn = taskFn_v;
  caml_register_global_root(&taskFn);

  dispatch_queue_t global =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
  dispatch_async(global, ^{
    brisk_caml_call(taskFn);
    caml_remove_global_root((value *)&taskFn);
  });
}

void ml_dispatchSyncMain(value taskFn_v) {
  CAMLparam1(taskFn_v);
  value taskFn = taskFn_v;
  caml_register_global_root(&taskFn);

  caml_release_runtime_system();

  dispatch_sync(dispatch_get_main_queue(), ^{
    brisk_caml_call(taskFn);
    caml_remove_global_root((value *)&taskFn);
  });

  caml_acquire_runtime_system();
}
