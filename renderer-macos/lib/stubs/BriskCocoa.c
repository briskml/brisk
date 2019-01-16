#import "BriskCocoa.h"
#import "BriskApplicationDelegate.h"
#import "BriskWindowDelegate.h"
#import <caml/threads.h>

dispatch_semaphore_t caml_thread_sema;

NSMutableSet *retainedViews;

void brisk_init() {
  retainedViews = [NSMutableSet new];
  caml_thread_sema = dispatch_semaphore_create(1);
}

void ml_isMain() { NSLog(@"%i", [NSThread isMainThread]); }

CAMLprim void run_lwt_iter_if_needed() {
  /*
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
  ^{ brisk_caml_call(^{ intnat should_schedule =
  caml_callback(*caml_named_value("Brisk_lwt_iter"), Val_unit); if
  (should_schedule == 1) { ml_lwt_iter();
      }
    });
    ml_lwt_iter();
  });
  */
}

void brisk_caml_call(void (^block)()) {
  dispatch_semaphore_wait(caml_thread_sema, DISPATCH_TIME_FOREVER);
  // This should only be called when we call from outside of OCaml I suppose
  caml_c_thread_register();
  caml_acquire_runtime_system();
  block();
  caml_release_runtime_system();
  dispatch_semaphore_signal(caml_thread_sema);
  run_lwt_iter_if_needed();
}

CAMLprim void ml_lwt_iter() {
  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
                 ^{
                   brisk_caml_call(^{
                     /*intnat should_schedule = */ caml_callback(
                         *caml_named_value("Brisk_lwt_iter"), Val_unit);
                     // if (should_schedule == 1) {
                     // }
                   });
                   sleep(2);
                   ml_lwt_iter();
                 });
}

void flush_and_layout_sync() {
  caml_callback(*caml_named_value("Brisk_flush_layout"), Val_unit);
}

void ml_schedule_layout_flush() {
  dispatch_async(dispatch_get_main_queue(), ^{
    brisk_caml_call(^{
      flush_and_layout_sync();
    });
  });
}

void brisk_caml_call_and_flush(void (^block)()) {
  brisk_caml_call(^{
    block();
    flush_and_layout_sync();
  });
}

void retainView(NSView *view) { [retainedViews addObject:view]; }

void releaseView(NSView *view) { [retainedViews removeObject:view]; }
