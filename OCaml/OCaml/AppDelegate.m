//
//  AppDelegate.m
//  OCaml
//
//  Created by Wojciech Czekalski on 26.11.2017.
//  Copyright © 2017 wokalski. All rights reserved.
//

#import "AppDelegate.h"
#import <caml/mlvalues.h>
#import <caml/memory.h>
#import <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/custom.h>
#include <caml/fail.h>

static NSMutableDictionary *PSM_Views;

CAMLprim value View_newView(value id_) {
    int _id = Int_val(id_);
    UIView *view = [UIView new];
    view.backgroundColor = [UIColor redColor];
    [view setFrame:CGRectMake(10, 10, 100, 100 )];
    view.layer.borderWidth = 10;
    view.layer.borderColor = [[UIColor blueColor] CGColor];
    [PSM_Views setObject:view forKey:@(_id)];
    return (value) view;
}

CAMLprim value View_getInstance(value id_) {
    int _id = Int_val(id_);
    UIView *view = [PSM_Views objectForKey:@(_id)];
    if (view) {
        CAMLparam0();
        CAMLlocal1( some );
        some = caml_alloc(1, 0);
        Store_field( some, 0, (long) view );
        CAMLreturn( some );
    } else {
        return Val_int(0);
    }
}

CAMLprim value View_setFrame(value x, value y, value width, value height, UIView *view) {
    double x_C = Double_val(x);
    double y_C = Double_val(y);
    double width_C = Double_val(width);
    double height_C = Double_val(height);
    [view setFrame:CGRectMake(x_C, y_C, width_C, height_C)];
    return (value) view;
}

CAMLprim value View_addToWindow(UIView *view) {
    [[[[UIApplication sharedApplication] windows] firstObject] addSubview:view];
    return (value) view;
}

CAMLprim value View_addChild(UIView *self, UIView *child /* array of UIViews */ ) {
    [self addSubview: child];
    return (value) self;
}

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    PSM_Views = [NSMutableDictionary new];
    // Override point for customization after application launch.
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
