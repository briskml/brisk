//
//  AppDelegate.m
//  Brisk
//
//  Created by Rauan Mayemir on 6/29/18.
//  Copyright © 2018 Brisk. All rights reserved.
//

#import "AppDelegate.h"
#import <caml/mlvalues.h>
#import <caml/memory.h>
#import <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/threads.h>
#include <caml/callback.h>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

static NSMutableDictionary *PSM_Views;
static NSMutableArray *PSM_Views_ALL;
NSView *rootView;
dispatch_queue_t q;

CAMLprim value View_newView() {
    NSView __block *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [NSView new];
        view.layer.borderWidth = 0.5;
        view.layer.borderColor = [[NSColor blackColor] CGColor];
    });
    [PSM_Views_ALL addObject:view];
    return (long) view;
}

CAMLprim value View_memoizeInstance(intnat id_, NSView *view) {
    [PSM_Views setObject:view forKey:@(id_)];
    return Val_unit;
}

CAMLprim value View_freeInstance(intnat id_) {
    [PSM_Views removeObjectForKey:@(id_)];
    return Val_unit;
}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Insert code here to tear down your application
}


@end
