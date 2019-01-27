#import "BriskCocoa.h"

@protocol BriskStylableText

@property(nonatomic, retain) NSMutableAttributedString *attributedString;
@property(nonatomic, retain) NSMutableDictionary *attributedProps;
@property(nonatomic, retain) NSMutableParagraphStyle *paragraphStyle;

- (void)applyTextStyle;

@end
