#import "BriskCocoa.h"

@protocol BriskStylableText

@property(nonatomic, retain) NSMutableParagraphStyle *brisk_paragraphStyle;

- (void)brisk_addAttribute:(NSAttributedStringKey)attributeName
               value:(id)value;
- (void)brisk_applyTextStyle;
- (void)brisk_setLineBreakMode:(NSLineBreakMode)mode;
@optional
- (void)brisk_beginTextStyleChanges;

@end
