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
#include <caml/callback.h>

typedef void (^ActionBlock)();

@interface UIBlockButton : UIButton {
    ActionBlock _actionBlock;
}

-(void) handleControlEvent:(UIControlEvents)event
                 withBlock:(ActionBlock) action;
@end

@implementation UIBlockButton

-(void) handleControlEvent:(UIControlEvents)event
                 withBlock:(ActionBlock) action
{
    _actionBlock = action;
    [self addTarget:self action:@selector(callActionBlock:) forControlEvents:event];
}

-(void) callActionBlock:(id)sender{
    _actionBlock();
}
@end

static NSMutableDictionary *PSM_Views;



CAMLprim value View_newView(value id_) {
    int _id = Int_val(id_);
    UIView __block *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [UIView new];
        view.backgroundColor = [UIColor redColor];
        [view setFrame:CGRectMake(10, 10, 100, 100 )];
        view.layer.borderWidth = 10;
        view.layer.borderColor = [[UIColor blueColor] CGColor];
    });
    [PSM_Views setObject:view forKey:@(_id)];
    return (value) view;
}

CAMLprim value Button_makeInstance(value id_) {
    int _id = Int_val(id_);
    UIView __block *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [UIBlockButton new];
        view.backgroundColor = [UIColor redColor];
        [view setFrame:CGRectMake(10, 10, 100, 100 )];
        view.layer.borderWidth = 10;
        view.layer.borderColor = [[UIColor blueColor] CGColor];
    });
    [PSM_Views setObject:view forKey:@(_id)];
    return (value) view;
}

CAMLprim value Button_setText(value text, UIBlockButton *view) {
    char *string = String_val(text);
    dispatch_sync(dispatch_get_main_queue(), ^{
        NSString *str = [NSString stringWithFormat:@"%s", string];
        [view setTitle:str forState:UIControlStateNormal];
    });
    return (value) view;
}

CAMLprim value Button_setCallback(value c, UIBlockButton *view) {
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view handleControlEvent:UIControlEventTouchDown withBlock:^{
            caml_callback(c, 0);
        }];
    });
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
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view setFrame:CGRectMake(x_C, y_C, width_C, height_C)];
    });
    return (value) view;
}

CAMLprim value View_getWindow() {
    UIWindow __block *windows;
    dispatch_sync(dispatch_get_main_queue(), ^{
        windows = [[[UIApplication sharedApplication] windows] firstObject];
    });
    return (value) windows;
}

CAMLprim value View_addChild(UIView *self, UIView *child /* array of UIViews */ ) {

        dispatch_sync(dispatch_get_main_queue(), ^{
            [self addSubview: child];
        });
    return (value) self;
}

CAMLprim value View_removeChild(UIView *self, UIView *child /* array of UIViews */ ) {
    if ([NSThread isMainThread])
    {
        NSLog(@"MAIN");
        [child removeFromSuperview];
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            NSLog(@"NOT MAIN");
            [child removeFromSuperview];
        });
    }
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
