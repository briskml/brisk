#import "Cocoa_stubs.h"
#define CAML_NAME_SPACE
#import <caml/alloc.h>

@interface TextView : NSTextView

@property(nonatomic, assign) NSMutableAttributedString *attributedString;
@property(nonatomic, assign) NSMutableDictionary *attributedProps;
@property(nonatomic, assign) NSMutableParagraphStyle *paragraphStyle;

@end

void ml_NSTextView_applyAttributes(TextView *txt);

double ml_NSTextView_getTextWidth(TextView *txt);
double ml_NSTextView_getTextHeight(TextView *txt);

CAMLprim value ml_NSTextView_getTextWidth_bc(TextView *txt);
CAMLprim value ml_NSTextView_getTextHeight_bc(TextView *txt);

CAMLprim value ml_NSTextView_setStringValue(TextView *txt, value str_v);
CAMLprim value ml_NSTextView_setFont(TextView *txt, value fontName_v,
                                     double fontSize, double fontWeight);

void ml_NSTextView_setColor(TextView *txt, double red, double green,
                            double blue, double alpha);
CAMLprim value ml_NSTextView_setColor_bc(TextView *txt, value red_v,
                                         value green_v, value blue_v,
                                         value alpha_v);

void ml_NSTextView_setBackgroundColor(TextView *txt, double red, double green,
                                      double blue, double alpha);
CAMLprim value ml_NSTextView_setBackgroundColor_bc(TextView *txt, value red_v,
                                                   value green_v, value blue_v,
                                                   value alpha_v);

void ml_NSTextView_setAlignment(TextView *txt, int align);
CAMLprim value ml_NSTextView_setAlignment_bc(TextView *txt, value align_v);

void ml_NSTextView_setLineBreakMode(TextView *txt, int mode);
CAMLprim value ml_NSTextView_setLineBreakMode_bc(TextView *txt, value mode_v);

void ml_NSTextView_setLineSpacing(TextView *txt, double spacing);
CAMLprim value ml_NSTextView_setLineSpacing_bc(TextView *txt, value spacing_v);

void ml_NSTextView_setKern(TextView *txt, double kern);
CAMLprim value ml_NSTextView_setKern_bc(TextView *txt, value kern_v);
