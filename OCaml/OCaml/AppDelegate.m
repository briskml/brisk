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
#include <caml/threads.h>
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
UIView *rootView;
dispatch_queue_t q;



CAMLprim value View_newView(value id_) {
    CAMLparam1(id_);
    int _id = Int_val(id_);
    UIView __block *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [UIView new];
        view.layer.borderWidth = 0.5;
        view.layer.borderColor = [[UIColor blackColor] CGColor];
    });
    [PSM_Views setObject:view forKey:@(_id)];
    CAMLreturn((long) view);
}

CAMLprim value Button_makeInstance(value id_) {
    CAMLparam1(id_);
    int _id = Int_val(id_);
    UIButton __block *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [UIBlockButton buttonWithType:UIButtonTypeSystem];
    });
    [PSM_Views setObject:view forKey:@(_id)];
    CAMLreturn((long)view);
}

CAMLprim value Button_setText(value text, UIBlockButton *view) {
    CAMLparam1(text);
    char *string = String_val(text);
    dispatch_sync(dispatch_get_main_queue(), ^{
        NSString *str = [NSString stringWithFormat:@"%s", string];
        [view setTitle:str forState:UIControlStateNormal];
    });
    CAMLreturn((long) view);
}

CAMLprim value Button_setCallback(value c, UIBlockButton *view) {
    CAMLparam1(c);
    value __block callback_ = c;
    caml_register_global_root(&callback_);
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view handleControlEvent:UIControlEventTouchUpInside withBlock:^{
            caml_callback(callback_, Val_unit);
        }];
    });
    CAMLreturn((long)view);
}

CAMLprim value View_getInstance(value id_) {
    CAMLparam1(id_);
    int _id = Int_val(id_);
    UIView *view = [PSM_Views objectForKey:@(_id)];
    if (view) {
        CAMLlocal1( some );
        some = caml_alloc(1, 0);
        Store_field( some, 0, (long) view );
        CAMLreturn( some );
    } else {
        CAMLreturn(Val_int(0));
    }
}

CAMLprim value View_setFrame(intnat x, intnat y, intnat width, intnat height, UIView *view) {
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view setFrame:CGRectMake(x, y, width, height)];
    });
    return Val_unit;
}

CAMLprim value View_setBackgroundColor(double red, double green, double blue, double alpha, UIView *view) {
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view setBackgroundColor:[UIColor colorWithRed:red green:green blue:blue alpha:alpha]];
    });
    return Val_unit;
}

CAMLprim value View_setBorderColor(double red, double green, double blue, double alpha, UIView *view) {
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view.layer setBorderColor:[[UIColor colorWithRed:red green:green blue:blue alpha:alpha] CGColor]];
    });
    return Val_unit;
}

CAMLprim value View_setBorderWidth(double width, UIView *view) {
    dispatch_sync(dispatch_get_main_queue(), ^{
        [view.layer setBorderWidth:width];
    });
    return Val_unit;
}

CAMLprim value View_getWindow() {
    return (value) rootView;
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

@interface BlockHolder : NSObject
@property (nonatomic, copy) void (^blockName)(void);
- (void)invokeBlock;
@end

@implementation BlockHolder
- (void)invokeBlock {
    self.blockName();
}
@end

static value callback_;

void CA_registerLoop(value c) {
    CAMLparam1(c);
    callback_ = c;
    caml_register_generational_global_root(&callback_);
    BlockHolder *holder = [BlockHolder new];
    holder.blockName = ^{
        dispatch_async(q, ^{
            caml_callback(callback_, 0);
        });
    };
    CADisplayLink *link = [CADisplayLink displayLinkWithTarget:holder selector:@selector(invokeBlock)];
    [link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    CAMLreturn0;
}

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    PSM_Views = [NSMutableDictionary new];
    rootView = [UIView new];
    q = dispatch_queue_create(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    [PSM_Views setObject:rootView forKey:@"ROOT"];
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
