#import "BriskStylableText.h"

@interface BriskTextInput : NSTextInputContext <BriskStylableText>

@end

@implementation NSTextInputContext

- (instancetype)initWithClient:(id<NSTextInputClient>)client {
  //designated initializer
}

- (BOOL)handleEvent:(NSEvent *)event {
  //if system consumes event return YES
}

- (void)activate {
  //invoked when receiver gets activated
}

- (void)deactivate {
  //invoked when receiver gets deactivated
}

- (void)discardMarkedText {
  //Notifies the system to discard the current conversion session. The client should clear its marked range when sending this message.
}

- (void)invalidateCharacterCoordinates {
  //Notifies the text input system that information related to character positions, including the document's visual coordinates, text selection, and document contents, has been modified. Text engines implementing the NSTextInputClient protocol should send this message whenever any of these changes occur, and -[NSTextInputContext handleEvent:] will not otherwise be called. -handleEvent: serves as an implicit notification that any of these changes could have occurred.
}

+ (nullable NSString *)localizedNameForInputSource:(NSTextInputSourceIdentifier)inputSourceIdentifier {
  /**** Text Input source attributes ****/
/* Returns the display name for inputSourceIdentifier.
 */
}

@dynamic currentInputContext;

@end

NSTextInputContext *ml_BriskTextInput_make() {
  BriskTextInput *input = [BriskTextInput new];
  retainView(input);

  return input;
}

