#import "BriskCocoa.h"
#import "BriskApplicationDelegate.h"
#import "BriskWindowDelegate.h"
#import <caml/threads.h>
#import <caml/unixsupport.h>

NSMutableSet *retainedViews;
int mainNeedsOCaml;
int fds[2];

void brisk_init() {
  retainedViews = [NSMutableSet new];
  mainNeedsOCaml = 0;
  pipe(fds);
}

void brisk_caml_call(void (^block)()) {
  // This should only be called when we call from outside of OCaml I suppose
  caml_c_thread_register();
  caml_acquire_runtime_system();
  block();
  caml_release_runtime_system();

  mainNeedsOCaml = 0;
}

intnat ml_shouldReleaseRuntime() { return mainNeedsOCaml; }

intnat ml_getMainFd() { return (intnat)fds[0]; }

void brisk_setNeedsRuntime() {
  printf("brisk_setNeedsRuntime\n");
  mainNeedsOCaml = 1;
  char buffer = 'a';
  write(fds[1], &buffer, 1);
}

void ml_runTaskInBackground(value taskFn_v) {
  CAMLparam1(taskFn_v);

  value taskFn = taskFn_v;

  printf("ml_runTaskInBackground\n");
  mainNeedsOCaml = NO;
  caml_register_global_root(&taskFn);

  dispatch_queue_t global =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
  dispatch_async(global, ^{
    brisk_caml_call(^{
      caml_callback(taskFn, Val_unit);
    });
    // caml_remove_global_root(&taskFn);
  });
}

void flush_and_layout_sync() {
  // caml_callback(*caml_named_value("Brisk_RunLoop_updateHostViewAndLayout"),
  //               Val_unit);
}

void ml_scheduleHostViewUpdateAndLayout(intnat fd) {
  printf("ml_scheduleHostViewUpdateAndLayout\n");
  caml_release_runtime_system();
  printf("dafuq?");
  dispatch_async(dispatch_get_main_queue(), ^{
    brisk_caml_call(^{
      flush_and_layout_sync();
      char buffer = 'a';
      printf("fd %ld", fd);
      write(fd, &buffer, 1);
    });
  });
}

void ml_is_main() { printf("isMainThread: %i\n", [NSThread isMainThread]); }

void brisk_caml_call_and_flush(void (^block)()) {
  brisk_caml_call(^{
    block();
    flush_and_layout_sync();
  });
}

void retainView(NSView *view) { [retainedViews addObject:view]; }

void releaseView(NSView *view) { [retainedViews removeObject:view]; }
