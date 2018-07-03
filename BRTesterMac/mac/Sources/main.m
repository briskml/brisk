//
//  main.m
//  Brisk
//
//  Created by Rauan Mayemir on 6/29/18.
//  Copyright © 2018 Brisk Contributors. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <caml/callback.h>

int main(int argc, const char * argv[])
{
    caml_main((char_os **) argv);
    value *react_run = caml_named_value("React.run");
    caml_callback(*react_run, Int_val(0));
    exit(0);
}
