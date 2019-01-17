#import "BriskCocoa.h"

@protocol BriskStylableText

@property(nonatomic, assign) NSMutableAttributedString *attributedString;
@property(nonatomic, assign) NSMutableDictionary *attributedProps;
@property(nonatomic, assign) NSMutableParagraphStyle *paragraphStyle;

- (void)applyTextStyle;

@end
