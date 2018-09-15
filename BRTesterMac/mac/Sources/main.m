//
//  main.m
//  Brisk
//
//  Created by Rauan Mayemir on 6/29/18.
//  Copyright © 2018 Brisk Contributors. All rights reserved.
//

#import <caml/callback.h>

int main(int argc, const char *argv[])
{
  caml_main((char_os **) argv);
  value *react_run = caml_named_value("React.run");
  caml_callback(*react_run, Val_unit);
  return 0;
}
