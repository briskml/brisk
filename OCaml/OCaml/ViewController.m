//
//  ViewController.m
//  OCaml
//
//  Created by Wojciech Czekalski on 26.11.2017.
//  Copyright © 2017 wokalski. All rights reserved.
//

#import "ViewController.h"
#import <caml/callback.h>

@implementation ViewController

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    char* args[] = {NULL};
    caml_startup(args);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
