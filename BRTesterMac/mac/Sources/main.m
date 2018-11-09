//
//  main.m
//  Brisk
//
//  Created by Rauan Mayemir on 6/29/18.
//  Copyright © 2018 Brisk Contributors. All rights reserved.
//

#import <caml/callback.h>
#import <caml/threads.h>
#import <Cocoa/Cocoa.h>

int main(int argc, const char *argv[])
{
  caml_main((char_os **) argv);
  caml_release_runtime_system();
  @autoreleasepool {
    [[NSApplication sharedApplication] run];
  }
  return 0;
}
