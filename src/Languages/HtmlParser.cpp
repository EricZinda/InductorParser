#include "HtmlParser.h"

namespace Html
{
    // tokens
    // attribute names cannot include the space characters, U+0000 NULL, """, "'", ">", "/", "=", the control characters, and any characters that are not defined by Unicode.
    char AttributeNameIllegalCharacters[] = "\r\n\t \"\'>/=";
    char BeginEndTagString[] = "</";
    char DoubleQuotedAttributeIllegalCharacters[] = "\"";
    char EndHtmlCommentString[] = "-->";
    char EndStyleTagString[] = "</style";
    char SingleQuotedAttributeIllegalCharacters[] = "'";
    char StartHtmlCommentString[] = "<!--";
    char StyleTagNameString[] = "style";
    char UnquotedAttributeIllegalCharacters[] = "\r\n\t \"\'<>/=`";

    // errors
    char errExpectedAttributeName[] = "Expected attribute name";
    char errExpectedElement[] = "Expected element";
    char errExpectedStartTag[] = "Expected start tag";
    char errExpectedTagName[] = "Expected tagname";
    char errExpectedProcessingInstruction[] = "Expected xml processing instruction";
}