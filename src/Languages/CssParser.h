#pragma once
#include "FXPlatform/Parser/Parser.h"
using namespace FXPlat;

// Background used for this example CSS parser
// http://www.w3.org/TR/CSS21/syndata.html#strings
// http://www.w3.org/TR/css3-syntax/#syntax-description

namespace Html
{
    // Tokens
    extern char BeginCommentBlock[];
    extern char CssLineContinuationString[];
    extern char EmString[];
    extern char EndCommentBlock[];
    extern char EscapedDoubleQuoteString[];
    extern char EscapedSingleQuoteString[];
    extern char PixelString[];
    extern char PointString[];
    extern char RgbaString[];
    extern char UrlString[];
    extern char ZeroString[];

    // Errors
    extern char errExpectedCssBlockComment[];
    extern char errExpectedCssSelectorIdentifierRule[];
    extern char errExpectedCssSelectorRule[];
    extern char errExpectedCssSelectorListRule[];
    extern char errExpectedCssDeclarationValueRule[];
    extern char errExpectedCssDeclaration[];
    extern char errExpectedCssRuleSymbol[];
    extern char errExpectedUrlExpressionRule[];

    class CssSymbolID
    {
    public:
		// Start IDs after CustomSymbolStart so they don't conflict with
		// built in IDs
        SymbolDef(cssBlockComment, CustomSymbolStart + 0);
        SymbolDef(identifier, CustomSymbolStart + 1);
        SymbolDef(classSelector, CustomSymbolStart + 2);
        SymbolDef(typeSelector, CustomSymbolStart + 3);
        SymbolDef(idSelector, CustomSymbolStart + 4);
        SymbolDef(selectorGroup, CustomSymbolStart + 5);
        SymbolDef(cssRule, CustomSymbolStart + 6);
        SymbolDef(cssLength, CustomSymbolStart + 7);
        SymbolDef(cssValue, CustomSymbolStart + 8);
        SymbolDef(cssProperty, CustomSymbolStart + 9);
        SymbolDef(cssDeclaration, CustomSymbolStart + 10);
        SymbolDef(cssDocument, CustomSymbolStart + 11);
        SymbolDef(cssDescendentCombinator, CustomSymbolStart + 12);
        SymbolDef(cssSelector, CustomSymbolStart + 13);
        SymbolDef(cssUniversalSelector, CustomSymbolStart + 14);
        SymbolDef(cssDeclarationGroup, CustomSymbolStart + 15);
        SymbolDef(cssValueRgba, CustomSymbolStart + 16);
        SymbolDef(cssValueString, CustomSymbolStart + 17);
        SymbolDef(cssValueUrl, CustomSymbolStart + 18);
        SymbolDef(cssArrayDelimiter, CustomSymbolStart + 19);
        SymbolDef(cssValueColorHex, CustomSymbolStart + 20);
        SymbolDef(cssUnitlessZeroLength, CustomSymbolStart + 22);
        SymbolDef(pseudoSelector, CustomSymbolStart + 23);
        SymbolDef(inlineStyleSelector, CustomSymbolStart + 24);
    };

	#define CssSymbolIDEnd CustomSymbolStart + 1000

    // Block Comment
    // /* comment */
    class CssBlockCommentRule : public 
        AndExpression<Args
        <
            LiteralExpression<BeginCommentBlock>,
            NotLiteralExpression<EndCommentBlock>,
            LiteralExpression<EndCommentBlock>
        >
        , FlattenType::Delete, SymbolID::andExpression, errExpectedCssBlockComment>
    {
    };

    // Whitespace
    // normal whitespace + comment
    class CssWhitespaceRule : public
        ZeroOrMoreExpression
        <
            OrExpression<Args
            <
                WhitespaceCharSymbol,
                CssBlockCommentRule
            >>,
            FlattenType::Delete
        >
    {
    };

    //  String
    // Strings can either be written with double quotes or with single quotes. Double quotes cannot occur inside double quotes, unless escaped (e.g., as '\"' or as '\22'). 
	// Analogously for single quotes (e.g., "\'" or "\27"). 
    // A string cannot directly contain a newline. To include a newline in a string, use an escape representing the line feed character in ISO-10646 (U+000A), 
	// such as "\A" or "\00000a". This character represents the generic 
	// notion of "newline" in CSS. See the 'content' property for an example. 
    // It is possible to break strings over several lines, for aesthetic or other reasons, but in such a case the newline itself has to be escaped with a backslash (\). For instance, the following two selectors are exactly the same:
    // CssDoubleQuoteString := " ( \" | \CRLF | AnyCharacterBut(") )* "\"" 
    typedef AndExpression<Args
        <
            CharacterSymbol<DoubleQuoteString>,
            ZeroOrMoreExpression
            <
                OrExpression<Args
                <
                    ReplaceExpression
                    <
                        LiteralExpression<EscapedDoubleQuoteString>,
                        DoubleQuoteString
                    >,
                    LiteralExpression<CssLineContinuationString, FlattenType::Delete>,
                    CharacterSetExceptSymbol<DoubleQuoteString>
                >>
            >,
            CharacterSymbol<DoubleQuoteString>
        >, FlattenType::None, CssSymbolID::cssValueString> CssDoubleQuoteString;

	typedef AndExpression<Args
        <
            CharacterSymbol<SingleQuoteString>,
            ZeroOrMoreExpression
            <
                OrExpression<Args
                <
                    ReplaceExpression
                    <
                        LiteralExpression<EscapedSingleQuoteString>,
                        SingleQuoteString
                    >,
                    LiteralExpression<CssLineContinuationString, FlattenType::Delete>,
                    CharacterSetExceptSymbol<SingleQuoteString>
                >>
            >,
            CharacterSymbol<SingleQuoteString>
        >, FlattenType::None, CssSymbolID::cssValueString> CssSingleQuoteString;

	typedef OrExpression<Args
            <
                CssSingleQuoteString,
                CssDoubleQuoteString
            >> CssValueString;

    // Identifier ::= (CharSymbol | UnderscoreSymbol) (CharSymbol | NumberSymbol | UnderscoreSymbol)*
    template<FlattenType flatten = FlattenType::None, unsigned short ID = CssSymbolID::identifier, char *staticErrorMessage = DefaultErrorMessage>
    class CssSelectorIdentifierRule : public
        AndExpression<Args
        <
            OrExpression<Args<CharSymbol, CharacterSymbol<Underscore, FlattenType::None>>>,
            ZeroOrMoreExpression
            <
                OrExpression<Args
                <
                    CharOrNumberSymbol, 
                    CharacterSymbol<Underscore, FlattenType::None>, 
                    CharacterSymbol<HyphenString, FlattenType::None>
                >>
            >
        >, flatten, ID, errExpectedCssSelectorIdentifierRule> 
    {
    };

    // Combinator
    // Whitespace
    class CssCombinator : public
        WhitespaceSymbol<FlattenType::None, CssSymbolID::cssDescendentCombinator>
    {
    };

    // ClassSelector
    // . CssSelectorIdentifierRule
    class CssClassSelectorRule : public
        AndExpression<Args<
            CharacterSymbol<PeriodString>,
            CssSelectorIdentifierRule<>
        >, FlattenType::None, CssSymbolID::classSelector>
    {
    };

    // IDSelector
    // # CssSelectorIdentifierRule
    class CssIDSelectorRule : public
        AndExpression<Args
        <
            CharacterSymbol<PoundString>,
            CssSelectorIdentifierRule<>
        >, FlattenType::None, CssSymbolID::idSelector>
    {
    };

    // TypeSelector
    // Identifier ::= (CharSymbol | UnderscoreSymbol) (CharSymbol | NumberSymbol | UnderscoreSymbol)*
    class CssTypeSelectorRule : public
        CssSelectorIdentifierRule<FlattenType::None, CssSymbolID::typeSelector>
    {
    };

    // PseudoSelector
    // ':' [ ':' ] CssSelectorIdentifierRule
    class CssPseudoSelectorRule : public
        AndExpression<Args
        <
            CharacterSymbol<ColonString>,
            OptionalExpression<CharacterSymbol<ColonString>>,
            CssSelectorIdentifierRule<>
        >, FlattenType::None, CssSymbolID::pseudoSelector>
    {
    };

    // SimpleSelectorSequence
    // (ClassSelector | IDSelector | PseudoSelector | typeSelector | *) (ClassSelector | PseudoSelector | IDSelector )*
    class CssSimpleSelectorSequence : public
        AndExpression<Args
        <
            OrExpression<Args<
                CssClassSelectorRule,
                CssIDSelectorRule,
                CssPseudoSelectorRule,
                CssTypeSelectorRule,
                GroupExpression
                <
                    CharacterSymbol<AsterixString, FlattenType::None>, FlattenType::None, CssSymbolID::cssUniversalSelector
                >
            >>,
            ZeroOrMoreExpression
            <
                OrExpression<Args<
                    CssClassSelectorRule,
                    CssPseudoSelectorRule,
                    CssIDSelectorRule
                >>
            >
        >>
    {
    };

    // A selector is a chain of one or more sequences of simple selectors separated by combinators. One pseudo-element may be appended to the last sequence 
    //    of simple selectors in a selector. 
    // A sequence of simple selectors is a chain of simple selectors that are not separated by a combinator. It always begins with a type selector or a 
    //    universal selector. No other type selector or universal selector is allowed in the sequence. 
    // A simple selector is either a type selector, universal selector, attribute selector, class selector, ID selector, or pseudo-class. 
    // Selector
    // SimpleSelectorSequence (Combinator SimpleSelectorSequence)*
    class CssSelectorRule : public 
        AndExpression<Args
        <
            CssSimpleSelectorSequence,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    CssCombinator,
                    CssSimpleSelectorSequence
                >>
            >
        >, FlattenType::None, CssSymbolID::cssSelector>
    {
    };

    // CssSelectorList
    // CssWhitespaceRule CssSelectorRule [CssWhitespaceRule, CssSelectorRule]
    class CssSelectorList: public
        AndExpression<Args
        <
            CssWhitespaceRule,
            CssSelectorRule,
            ZeroOrMoreExpression
            <
                AndExpression<Args<
                    CssWhitespaceRule,
                    CharacterSymbol<CommaString>,
                    CssWhitespaceRule,
                    CssSelectorRule
                >, FlattenType::Flatten>
            >
        >, FlattenType::Flatten, SymbolID::andExpression, errExpectedCssSelectorListRule>
    {
    };

    // CssValueUrl
    // "url" "(" """ AnythingBut" """ ")" | "(" AnythingBut) ")"
    class CssValueUrl : public
        OrExpression<Args
        <
            AndExpression<Args
            <
                LiteralExpression<UrlString, FlattenType::Delete>,
                CharacterSymbol<LeftParenthesisString, FlattenType::Delete>,
                CharacterSymbol<DoubleQuoteString, FlattenType::Delete>,
                NotLiteralExpression<DoubleQuoteString>,
                CharacterSymbol<DoubleQuoteString, FlattenType::Delete>,
                CharacterSymbol<RightParenthesisString, FlattenType::Delete>
            >>,
            AndExpression<Args
            <
                LiteralExpression<UrlString, FlattenType::Delete>,
                CharacterSymbol<LeftParenthesisString, FlattenType::Delete>,
                NotLiteralExpression<RightParenthesisString>,
                CharacterSymbol<RightParenthesisString, FlattenType::Delete>
            >>
        >, FlattenType::None, CssSymbolID::cssValueUrl, errExpectedUrlExpressionRule>
    {
    };

    // CssValueColorHex
    // #rgb || #rrggbb
    // "#" (Exactly3(HexNumber) | Exactly6(HexNumber))
    class CssValueColorHex : public
        AndExpression<Args
        <
            CharacterSymbol<PoundString, FlattenType::Delete>,
            OrExpression<Args
            <
                AtLeastAndAtMostExpression<HexNumberSymbol, 3, 3>,
                AtLeastAndAtMostExpression<HexNumberSymbol, 6, 6>
                >>
        >, FlattenType::None, CssSymbolID::cssValueColorHex>
    {
    };

    // CssValueRgba
    // "rgba" "(" CssWhitespaceRule Integer CssWhitespaceRule "," CssWhitespaceRule Integer CssWhitespaceRule "," CssWhitespaceRule Integer CssWhitespaceRule "," 
    // CssWhitespaceRule Float CssWhitespaceRule ")"
    class CssValueRgba : public
        AndExpression<Args
        <
            LiteralExpression<RgbaString, FlattenType::Delete>,
            CharacterSymbol<LeftParenthesisString>,
            CssWhitespaceRule,
            Integer<>,
            CssWhitespaceRule,
            CharacterSymbol<CommaString>,
            CssWhitespaceRule,
            Integer<>,
            CssWhitespaceRule,
            CharacterSymbol<CommaString>,
            CssWhitespaceRule,
            Integer<>,
            CssWhitespaceRule,
            CharacterSymbol<CommaString>,
            CssWhitespaceRule,
            Float<>,
            CssWhitespaceRule,
            CharacterSymbol<RightParenthesisString>
        >, FlattenType::None, CssSymbolID::cssValueRgba>
    {
    };

    class CssValueNumber : public
        OrExpression<Args
        <
            Float<>,
            Integer<>
        >, FlattenType::Flatten>
    {
    };

    // CssLengthValueRule
    // Number ["px" | "%" | "em" | "pt"] | 0
    class CssLengthValueRule : public
        OrExpression<Args
        <
            AndExpression<Args
            <
                CssValueNumber,
                OrExpression<Args
                <
                    LiteralExpression<PixelString>,
                    LiteralExpression<PointString>,
                    LiteralExpression<PercentString>,
                    LiteralExpression<EmString>
                >, FlattenType::None, CssSymbolID::cssLength>
            >>,
            GroupExpression
            <
                CharacterSymbol<ZeroString, FlattenType::None>,
                FlattenType::None, CssSymbolID::cssUnitlessZeroLength
            >
        >>
    {
    };

    // CssDeclarationValue
    // ColorHex | Rgba | Url | Length | Number | String | Identifier  
    class CssDeclarationValueRule : public
        OrExpression<Args
        <
            CssValueColorHex,
            CssValueRgba,
            CssValueUrl,
            CssLengthValueRule,
            CssValueNumber,
            CssValueString,
            CssSelectorIdentifierRule<>
        >, FlattenType::None, CssSymbolID::cssValue, errExpectedCssDeclarationValueRule>
    {
    };

    // Regular declaration or Shorthand Property
    // [CssSelectorIdentifierRule CssWhitespaceRule ":" CssWhitespaceRule (CssDeclarationValueRule CssWhitespaceRule)+] ";"
    class CssDeclaration : public
        AndExpression<Args
        <
            OptionalExpression
            <
                AndExpression<Args
                <
                    CssSelectorIdentifierRule<FlattenType::None, CssSymbolID::cssProperty>,
                    CssWhitespaceRule,
                    CharacterSymbol<ColonString>,
                    CssWhitespaceRule,
                    OneOrMoreExpression
                    <
                        AndExpression<Args
                        <
                            CssDeclarationValueRule,
                            CssWhitespaceRule,
                            OptionalExpression
                            <
                                AndExpression<Args
                                <
                                    LiteralExpression<CommaString, FlattenType::None, CssSymbolID::cssArrayDelimiter>,
                                    CssWhitespaceRule
                                >>
                            >
                        >>
                    >
                >>
            >,
            CharacterSymbol<SemicolonString>
        >, FlattenType::None, CssSymbolID::cssDeclaration, errExpectedCssDeclaration>
    {
    };

    // CssRuleSymbol
    // CssSelectorList CssWhitespaceRule { *(CssWhitespaceRule CssDeclaration) }
    class CssRuleSymbol : public
        AndExpression<Args
        <
            GroupExpression<CssSelectorList, FlattenType::None, CssSymbolID::selectorGroup>,
            CssWhitespaceRule,
            CharacterSymbol<LeftBraceString>,
            CssWhitespaceRule,
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    CssWhitespaceRule,
                    CssDeclaration
                >>, FlattenType::None, CssSymbolID::cssDeclarationGroup
            >,
            CssWhitespaceRule,
            CharacterSymbol<RightBraceString>
        >, FlattenType::None, CssSymbolID::cssRule, errExpectedCssRuleSymbol>
    {
    };

    // CssDocument
    // *(CssWhitespaceRule CssRuleSymbol) CssWhitespaceRule
    class CssDocument : public
        AndExpression<Args
        <
            ZeroOrMoreExpression
            <
                AndExpression<Args
                <
                    CssWhitespaceRule,
                    CssRuleSymbol
                >>
            >,
            CssWhitespaceRule,
            EofSymbol
        >, FlattenType::None, CssSymbolID::cssDocument>
    {
    };
}