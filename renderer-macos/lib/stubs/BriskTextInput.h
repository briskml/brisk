#import "BriskCocoa.h"

@protocol BriskTextInputProtocol <NSTextInput>

@property(nonatomic, retain) NSMutableAttributedString *attributedString;
@property(nonatomic, retain) NSMutableDictionary *attributedProps;
@property(nonatomic, retain) NSMutableParagraphStyle *paragraphStyle;
@property(nonatomic, retain) NSMutableAttributedString *placeholder;

- (void)applyTextStyle;

@end
