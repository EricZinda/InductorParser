#pragma once
#include "CssParser.h"
using namespace FXPlat;

// http://software.hixie.ch/utilities/js/live-dom-viewer/
// https://dvcs.w3.org/hg/domcore/raw-file/tip/Overview.html
// http://www.w3.org/TR/html-markup/syntax.html#void-element
// DocType:
//    normal doctype
//    deprecated doctype
//    legacy-tool-compatible doctype
    
// Attributes:
//    empty attribute syntax
//    unquoted attribute-value syntax
//    single-quoted attribute-value syntax
//    double-quoted attribute-value syntax

// Character Data:
//    normal character data
//    replaceable character data
//    non-replaceable character data

// Character References
//    named character references
//    decimal numeric character references
//    hexadecimal numeric character references

namespace Html
{
    class ElementRule;

    // Tokens
    extern char AttributeNameIllegalCharacters[];
    extern char BeginEndTagString[];
    extern char DoubleQuotedAttributeIllegalCharacters[];
    extern char EndHtmlCommentString[];
    extern char EndStyleTagString[];
    extern char SingleQuotedAttributeIllegalCharacters[];
    extern char StartHtmlCommentString[];
    extern char StyleTagNameString[];
    extern char UnquotedAttributeIllegalCharacters[];

    // Errors
    extern char errExpectedTagName[];
    extern char errExpectedAttributeName[];
    extern char errExpectedElement[];
    extern char errExpectedStartTag[];
    extern char errExpectedProcessingInstruction[];

    class HtmlSymbolID
    {
    public:
        SymbolDef(tagName, CssSymbolIDEnd + 0);
        SymbolDef(attributeName, CssSymbolIDEnd + 1);
        SymbolDef(attributeValue, CssSymbolIDEnd + 2);
        SymbolDef(element, CssSymbolIDEnd + 3);
        SymbolDef(startTag, CssSymbolIDEnd + 4);
        SymbolDef(attributes, CssSymbolIDEnd + 5);
        SymbolDef(voidElement, CssSymbolIDEnd + 6);
        SymbolDef(attribute, CssSymbolIDEnd + 7);
        SymbolDef(text, CssSymbolIDEnd + 8);
        SymbolDef(endTag, CssSymbolIDEnd + 9);
        SymbolDef(nonReplaceableTextElement, CssSymbolIDEnd + 10);
        SymbolDef(processingInstruction, CssSymbolIDEnd + 11);
    };
#define HtmlSymbolIDEnd (CssSymbolIDEnd + 1000)
    
    //character references are a form of markup for representing single individual characters. There are three types of character references:
    //    named character references
    //    decimal numeric character references
    //    hexadecimal numeric character references

    // Named Character Reference
    // Named character references consist of the following parts in exactly the following order:
    //    An "&" character.
    //    One of the names listed in the “Named character references” section of the HTML5 specification [HTML5], using the same case.
    //    A ";" character.

    //Decimal numerical character references consist of the following parts, in exactly the following order.
    //An "&" character.
    //A "#" character.
    //One or more digits in the range 0–9, representing a base-ten integer that itself is a Unicode code point that is not U+0000, U+000D, in the range U+0080–U+009F, or in the range 0xD8000–0xDFFF (surrogates).
    //A ";" character.

    //Hexadecimal numeric character references consist of the following parts, in exactly the following order.
    //An "&" character.
    //A "#" character.
    //Either a "x" character or a "X" character.
    //One or more digits in the range 0–9, a–f, and A–F, representing a base-sixteen integer that itself is a Unicode code point that is not U+0000, U+000D, in the range U+0080–U+009F, or in the range 0xD800–0xDFFF (surrogates).
    //A ";" character.
    
    // Tag names are used within element start tags and end tags to give the element’s name. HTML elements all have 
    // names that only use characters in the range 0–9, a–z, and A–Z.
    // TagName ::= (CharSymbol | NumberSymbol)+
    typedef OneOrMoreExpression
        <
            CharOrNumberSymbol, 
            FlattenType::None, 
            HtmlSymbolID::tagName, 
            errExpectedTagName
        > TagNameRule;

    // attribute names must consist of one or more characters other than the space characters, U+0000 NULL, """, "'", ">", "/", "=", the control characters, 
    // and any characters that are not defined by Unicode.
    // AttributeName ::= (AnythingExcept(...))+
    typedef OneOrMoreExpression
        <
            CharacterSetExceptSymbol<AttributeNameIllegalCharacters, FlattenType::None>,
            FlattenType::None,
            HtmlSymbolID::attributeName,
            errExpectedAttributeName
        > AttributeNameRule;

    // empty attribute syntax
    // EmptyAttribute ::= AttributeName
    typedef
        GroupExpression
        <
            AttributeNameRule,
            FlattenType::None, HtmlSymbolID::attribute
        >
        EmptyAttributeRule;

    //An unquoted attribute value is specified by providing the following parts in exactly the following order:
    //    an attribute name
    //    zero or more space characters
    //    a single "=" character
    //    zero or more space characters
    //    an attribute value
    //In addition to the general requirements for attribute values, an unquoted attribute value has the following restrictions:
    //    must not contain any literal space characters
    //    must not contain any """, "'", "=", ">", "<", or "`", characters
    //    must not be the empty string
    // UnquotedAttributeValueAttribute ::= AttributeName OptionalWhitespace "=" OptionalWhitespace (AnythingBut(...))+
    typedef AndExpression<Args
        <
            AttributeNameRule,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<EqualString>,
            OptionalWhitespaceSymbol<>,
            OneOrMoreExpression
            <
                CharacterSetExceptSymbol<UnquotedAttributeIllegalCharacters>, FlattenType::None, HtmlSymbolID::attributeValue>
            >,
            FlattenType::None, HtmlSymbolID::attribute
        > UnquotedAttributeValueAttributeRule;


    //single-quoted attribute-value syntax
    //A single-quoted attribute value is specified by providing the following parts in exactly the following order:
    //    an attribute name
    //    zero or more space characters
    //    a "=" character
    //    zero or more space characters
    //    a single "'" character
    //    an attribute value
    //    a "'" character.
    //In addition to the general requirements for attribute values, a single-quoted attribute value has the following restriction:
    //    must not contain any literal "'" characters
    // SingleQuotedAttributeValueAttribute ::= AttributeName OptionalWhitespace "=" OptionalWhitespace "'" AnythingBut("'"...) "'"
    // DoubleQuotedAttributeValueAttribute ::= AttributeName OptionalWhitespace "=" OptionalWhitespace "\"" AnythingBut("\""...) "\""
    typedef AndExpression<Args
        <
            AttributeNameRule,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<EqualString>,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<SingleQuoteString>,
            ZeroOrMoreExpression
            <
                CharacterSetExceptSymbol<SingleQuotedAttributeIllegalCharacters>, 
                FlattenType::None, 
                HtmlSymbolID::attributeValue
            >,
            CharacterSymbol<SingleQuoteString>
        >, FlattenType::None, HtmlSymbolID::attribute> SingleQuotedAttributeValueAttributeRule;

    typedef AndExpression<Args
        <
            AttributeNameRule,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<EqualString>,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<DoubleQuoteString>,
            ZeroOrMoreExpression
            <
                CharacterSetExceptSymbol<DoubleQuotedAttributeIllegalCharacters>, 
                FlattenType::None, 
                HtmlSymbolID::attributeValue
            >,
            CharacterSymbol<DoubleQuoteString>
        >, FlattenType::None, HtmlSymbolID::attribute> DoubleQuotedAttributeValueAttributeRule;

    // Attributes:
    //  attribute names must consist of one or more characters other than the space characters, U+0000 NULL, """, "'", ">", "/", "=", 
    //    the control characters, and any characters that are not defined by Unicode.
    //  attribute values can contain text and character references, with additional restrictions depending on whether they are unquoted attribute values, 
    //    single-quoted attribute values, or double-quoted attribute values. Also, the HTML elements section of this reference describes further restrictions
    //    on the allowed values of particular attributes, and attributes must have values that conform to those restrictions.
    //    empty attribute syntax
    //    unquoted attribute-value syntax
    //    single-quoted attribute-value syntax
    //    double-quoted attribute-value syntax
    typedef OrExpression<Args
        <
            DoubleQuotedAttributeValueAttributeRule,
            SingleQuotedAttributeValueAttributeRule,
            UnquotedAttributeValueAttributeRule,
            EmptyAttributeRule
        >> AttributeRule;

    // start tags consist of the following parts, in exactly the following order:
    //     A "<" character.
    //    The element’s tag name.
    //    Optionally, one or more attributes, each of which must be preceded by one or more space characters.
    //    Optionally, one or more space characters.
    //    Optionally, a "/" character, which may be present only if the element is a void element.
    //    A ">" character.
    // StartTag
    // "<" TagNameRule (Whitespace Attribute)* OptionalWhitespace ["/"] ">"
    typedef AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            TagNameRule,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    OptionalWhitespaceSymbol<>,
                    AttributeRule
                >>,
                FlattenType::None, HtmlSymbolID::attributes
            >,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<GreaterThanString>
        >, FlattenType::None, HtmlSymbolID::startTag, errExpectedStartTag> StartTagRule;

    template<char *tagName>
    class StartSpecificTagRule : public AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            LiteralExpression<tagName, FlattenType::None, HtmlSymbolID::tagName>,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    OptionalWhitespaceSymbol<>,
                    AttributeRule
                >>,
                FlattenType::None, HtmlSymbolID::attributes
            >,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<GreaterThanString>
        >, FlattenType::None, HtmlSymbolID::startTag, errExpectedStartTag>
    {
    };
    
    // void start tag
    typedef AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            TagNameRule,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    OptionalWhitespaceSymbol<>,
                    AttributeRule
                >>,
                FlattenType::None, HtmlSymbolID::attributes
            >,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<ForwardSlashString>,
            CharacterSymbol<GreaterThanString>
        >, FlattenType::None, HtmlSymbolID::startTag, errExpectedStartTag> VoidStartTagRule;

    // XML Processing Instruction: <? blah blah ?>
    typedef AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            CharacterSymbol<QuestionMarkString>,
            TagNameRule,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    OptionalWhitespaceSymbol<>,
                    AttributeRule
                >>,
                FlattenType::None, HtmlSymbolID::attributes
            >,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<QuestionMarkString>,
            CharacterSymbol<GreaterThanString>
    >, FlattenType::None, HtmlSymbolID::processingInstruction, errExpectedProcessingInstruction> ProcessingInstructionRule;
    
    // end tags consist of the following parts, in exactly the following order:
    //    A "<" character.
    //    A "/" character
    //    The element’s tag name.
    //    Optionally, one or more space characters.
    //    A ">" character.
    // EndTag
    // "<" "/" TagNameRule OptionalWhitespace ">"
    typedef AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            CharacterSymbol<ForwardSlashString>,
            TagNameRule,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<GreaterThanString>
        >, FlattenType::None, HtmlSymbolID::endTag> EndTagRule;

    template<char *tagName>
    class EndSpecificTagRule : public AndExpression<Args
        <
            CharacterSymbol<LessThanString>,
            CharacterSymbol<ForwardSlashString>,
            LiteralExpression<tagName, FlattenType::None, HtmlSymbolID::tagName>,
            OptionalWhitespaceSymbol<>,
            CharacterSymbol<GreaterThanString>
        >, FlattenType::None, HtmlSymbolID::endTag>
    {
    };

    // void Element
    typedef GroupExpression
        <
            VoidStartTagRule,
            FlattenType::None, HtmlSymbolID::voidElement
        >
        VoidElementRule;

    //comments consist of the following parts, in exactly the following order:
    //    the comment start delimiter "<!--"
    //    text
    //    the comment end delimiter "-->"
    //The text part of comments has the following restrictions:
    //    must not start with a ">" character
    //    must not start with the string "->"
    //    must not contain the string "--"
    //    must not end with a "-" character
    // Comment ::= "<!--" (Anything but "-->") "-->"
    typedef AndExpression<Args
        <
            LiteralExpression<StartHtmlCommentString>,
            NotLiteralExpression<EndHtmlCommentString>,
            LiteralExpression<EndHtmlCommentString>
        >, FlattenType::Delete> CommentRule;

    // NormalCharacterElement
    // Certain elements contain normal character data. Normal character data can contain the following:
    //      text
    //      character references
    // NormalCharacterElement ::= StartTag (Element | (Anything But <)+)* EndTag
    typedef AndExpression<Args
        <
            StartTagRule,
            ZeroOrMoreExpression
            <
                OrExpression<Args
                <
                    ElementRule,
                    OneOrMoreExpression
                    <
                        CharacterSetExceptSymbol<LessThanString>,
                        FlattenType::None, HtmlSymbolID::text
                    >
                >>
            >,
            EndTagRule
        >, FlattenType::None, HtmlSymbolID::element, errExpectedElement> NormalCharacterElementRule;

    // ReplaceableCharacterElement
    // In documents in the HTML syntax, the title and textarea elements can contain replaceable character data. Replaceable character data can 
    // contain the following:
    //    text, optionally including "<" characters
    //    character references
    // Replaceable character data has the following restrictions:
    //    must not contain any ambiguous ampersands
    //    must not contain any occurrences of the string "</" followed by characters that are a case-insensitive match for 
    //      the tag name of the element containing the replaceable character data (for example, "</title" or "</textarea"), followed by a space character, ">", or "/".
    // ReplaceableCharacterElement :== "<title" OptionalWhitespace ">" (CharacterReference | (Anything but "</title"))* "</title" OptionalWhitespace ">"

    //In documents in the HTML syntax, the script, and style elements can contain non-replaceable character data. Non-replaceable character data can contain the following:
    //    text, optionally including "<" characters
    //    ambiguous ampersands
    //Non-replaceable character data has the following restrictions:
    //    must not contain character references
    //    must not contain any occurrences of the string "</", followed by characters that are a case-insensitive match for the tag name of the element containing the 
    //      replaceable character data (for example, "</script" or "</style", followed by a space character, ">", or "/".
    // NonReplaceableCharacterElement :== "<script" OptionalWhitespace ">" (Anything but "</script>")* "</script>" OptionalWhitespace ">"
    typedef AndExpression<Args
        <
            StartSpecificTagRule<StyleTagNameString>,
            NotLiteralExpression<EndStyleTagString, FlattenType::None, HtmlSymbolID::text>,
            EndSpecificTagRule<StyleTagNameString>
        >, FlattenType::None, HtmlSymbolID::nonReplaceableTextElement, errExpectedElement> NonReplaceableCharacterElementRule;

    /*  Elements:
    start tags consist of the following parts, in exactly the following order:
    end tags consist of the following parts, in exactly the following order:
    Void elements only have a start tag; end tags must not be specified for void elements.
    The start and end tags of certain elements can be omitted. The subsection for each element in the HTML elements section of this reference provides information about which tags (if any) can be omitted for that particular element.
    A non-void element must have an end tag, unless the subsection for that element in the HTML elements section of this reference indicates that its end tag can be omitted.
    The contents of an element must be placed between just after its start tag (which might be implied, in certain cases) and just before its end tag (which might be implied in certain cases).*/
    // In documents in the HTML syntax, the title and textarea elements can contain replaceable character data.
    // In documents in the HTML syntax, the script, and style elements can contain non-replaceable character data

    // Element
    // Element ::= ReplaceableCharacterNode | NonReplaceableCharacterNode | NormalCharacterElementRule | CommentNode
    class ElementRule : public OrExpression<Args
        <
            CommentRule,
            NonReplaceableCharacterElementRule,
            NormalCharacterElementRule,
            VoidElementRule
        >, FlattenType::Flatten>
    {
    };

    //Documents must consist of the following parts, in the given order:
    //Optionally, a single "BOM" (U+FEFF) character.
    //Any number of comments and space characters.
    //A DOCTYPE.
    //Any number of comments and space characters.
    //The root element, in the form of an html element.
    //Any number of comments and space characters.

    // Html Document
    typedef AndExpression<Args
        <
            ZeroOrMoreExpression<
                OrExpression<Args<
                    WhitespaceSymbol<>,
                    CommentRule
                >>
            >,
            ElementRule,
            OptionalWhitespaceSymbol<>,
            EofSymbol
        >>
        HtmlDocumentRule;
}
