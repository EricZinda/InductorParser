#include "CssParser.h"

namespace Html
{
    char BeginCommentBlock[] = "/*";
    char CssLineContinuationString[] = "\\\r\n";
    char EndCommentBlock[] = "*/";
    char EscapedDoubleQuoteString[] = "\\\"";
    char EscapedSingleQuoteString[] = "\\'";

    char EmString[] = "em";
    char PointString[] = "pt";
    char PixelString[] = "px";
    char RgbaString[] = "rgba";
    char UrlString[] = "url";
    char ZeroString[] = "0";

    // Errors
    char errExpectedCssBlockComment[] = "expected /* comment */";
    char errExpectedCssSelectorIdentifierRule[] = "expected selector";
    char errExpectedCssSelectorRule[] = "expected .selector, #selector or selector";
    char errExpectedCssSelectorListRule[] = "expected selector, selector, etc";
    char errExpectedCssDeclarationValueRule[] = "expected Css property value";
    char errExpectedCssDeclaration[] = "expected 'property:value;'";
    char errExpectedCssRuleSymbol[] = "expected 'selector { property:value; }'";
    char errExpectedUrlExpressionRule[] = "expected url() function";
}
