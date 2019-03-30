#import "BriskCocoa.h"
#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CFBundle.h>

void ml_BriskCoreFoundation_openURL(value url_v) {
  CAMLparam1(url_v);

  char *utf8string = String_val(url_v);
  CFURLRef url = CFURLCreateWithBytes(NULL,                   // allocator
                                      (UInt8 *)utf8string,    // URLBytes
                                      strlen(utf8string),     // length
                                      kCFStringEncodingASCII, // encoding
                                      NULL                    // baseURL
  );
  LSOpenCFURLRef(url, 0);
  CFRelease(url);
}
