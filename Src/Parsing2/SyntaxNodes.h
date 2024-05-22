#pragma once

#include "./SyntaxBase.h"

#define abstract

namespace Alchemy::Compilation {

    struct ParameterListSyntax;
    struct BlockSyntax;
    struct WhenClauseSyntax;

    struct EmptyStatementSyntax : StatementSyntax {

        SyntaxToken semicolon;

        explicit EmptyStatementSyntax(SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::EmptyStatement)
            , semicolon(semicolon) {}

    };

    struct BreakStatementSyntax : StatementSyntax {

        SyntaxToken breakKeyword;
        SyntaxToken semicolon;

        BreakStatementSyntax(SyntaxToken breakKeyword, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::BreakStatement)
            , breakKeyword(breakKeyword)
            , semicolon(semicolon) {}

    };
    struct ContinueStatementSyntax : StatementSyntax {

        SyntaxToken continueKeyword;
        SyntaxToken semicolon;

        ContinueStatementSyntax(SyntaxToken continueKeyword, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::ContinueStatement)
            , continueKeyword(continueKeyword)
            , semicolon(semicolon) {}

    };

    struct VariableDeclarationSyntax;

    struct ForStatementSyntax : StatementSyntax {
        // declaration and initializers are mutually exclusive

        SyntaxToken forKeyword;
        SyntaxToken openParenToken;
        VariableDeclarationSyntax* declaration;
        SeparatedSyntaxList<ExpressionSyntax>* initializers;
        SyntaxToken firstSemiColon;
        ExpressionSyntax* condition;
        SyntaxToken secondSemiColon;
        SeparatedSyntaxList<ExpressionSyntax>* incrementors;
        SyntaxToken closeParenToken;
        StatementSyntax* statement;


        ForStatementSyntax(SyntaxToken forKeyword, SyntaxToken openParenToken, VariableDeclarationSyntax* declaration, SeparatedSyntaxList<ExpressionSyntax>* initializers, SyntaxToken firstSemiColon, ExpressionSyntax* condition, SyntaxToken secondSemiColon, SeparatedSyntaxList<ExpressionSyntax>* incrementors, SyntaxToken closeParenToken, StatementSyntax* statement)
            : StatementSyntax(SyntaxKind::ForStatement)
            , forKeyword(forKeyword)
            , openParenToken(openParenToken)
            , declaration(declaration)
            , initializers(initializers)
            , firstSemiColon(firstSemiColon)
            , condition(condition)
            , secondSemiColon(secondSemiColon)
            , incrementors(incrementors)
            , closeParenToken(closeParenToken)
            , statement(statement) {}

    };

    struct ThrowStatementSyntax : StatementSyntax {

        SyntaxToken throwKeyword;
        ExpressionSyntax* expression;
        SyntaxToken semicolon;

        ThrowStatementSyntax(SyntaxToken throwKeyword, ExpressionSyntax* expression, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::ThrowStatement)
            , throwKeyword(throwKeyword)
            , expression(expression)
            , semicolon(semicolon) {}

    };

    struct CatchDeclarationSyntax : SyntaxBase {

        SyntaxToken openParen;
        TypeSyntax* type;
        SyntaxToken identifier;
        SyntaxToken closeParen;

        CatchDeclarationSyntax(SyntaxToken openParen, TypeSyntax* type, SyntaxToken identifier, SyntaxToken closeParen)
            : SyntaxBase(SyntaxKind::CatchDeclaration)
            , openParen(openParen)
            , type(type)
            , identifier(identifier)
            , closeParen(closeParen) {}

    };

    struct CatchFilterClauseSyntax : SyntaxBase {

        SyntaxToken whenKeyword;
        SyntaxToken openParenToken;
        ExpressionSyntax* filterExpression;
        SyntaxToken closeParenToken;

        CatchFilterClauseSyntax(SyntaxToken whenKeyword, SyntaxToken openParenToken, ExpressionSyntax* filterExpression, SyntaxToken closeParenToken)
            : SyntaxBase(SyntaxKind::CatchFilterClause)
            , whenKeyword(whenKeyword)
            , openParenToken(openParenToken)
            , filterExpression(filterExpression)
            , closeParenToken(closeParenToken) {}

    };

    struct CatchClauseSyntax : SyntaxBase {

        SyntaxToken catchKeyword;
        CatchDeclarationSyntax* declaration;
        CatchFilterClauseSyntax* filter;
        BlockSyntax* block;

        CatchClauseSyntax(SyntaxToken catchKeyword, CatchDeclarationSyntax* declaration, CatchFilterClauseSyntax* filter, BlockSyntax* block)
            : SyntaxBase(SyntaxKind::CatchClause)
            , catchKeyword(catchKeyword)
            , declaration(declaration)
            , filter(filter)
            , block(block) {}

    };

    struct FinallyClauseSyntax : SyntaxBase {

        SyntaxToken finallyKeyword;
        BlockSyntax* block;

        FinallyClauseSyntax(SyntaxToken finallyKeyword, BlockSyntax* block)
            : SyntaxBase(SyntaxKind::FinallyClause)
            , finallyKeyword(finallyKeyword)
            , block(block) {}

    };

    struct TryStatementSyntax : StatementSyntax {

        SyntaxToken tryKeyword;
        BlockSyntax* tryBlock;
        SyntaxList<CatchClauseSyntax>* catchClauses;
        FinallyClauseSyntax* finallyClaus;

        TryStatementSyntax(SyntaxToken tryKeyword, BlockSyntax* tryBlock, SyntaxList<CatchClauseSyntax>* catchClauses, FinallyClauseSyntax* finallyClaus)
            : StatementSyntax(SyntaxKind::TryStatement)
            , tryKeyword(tryKeyword)
            , tryBlock(tryBlock)
            , catchClauses(catchClauses)
            , finallyClaus(finallyClaus) {}

    };

    struct DefaultSwitchLabelSyntax : SwitchLabelSyntax {

        SyntaxToken keyword;
        SyntaxToken colon;

        DefaultSwitchLabelSyntax(SyntaxToken keyword, SyntaxToken colon)
            : SwitchLabelSyntax(SyntaxKind::DefaultSwitchLabel)
            , keyword(keyword)
            , colon(colon) {}


    };

    struct CaseSwitchLabelSyntax : SwitchLabelSyntax {

        SyntaxToken keyword;
        ExpressionSyntax* value;
        SyntaxToken colon;

        CaseSwitchLabelSyntax(SyntaxToken keyword, ExpressionSyntax* value, SyntaxToken colon)
            : SwitchLabelSyntax(SyntaxKind::CaseSwitchLabel)
            , keyword(keyword)
            , value(value)
            , colon(colon) {}
    };

    struct CasePatternSwitchLabelSyntax : SwitchLabelSyntax {

        SyntaxToken keyword;
        PatternSyntax* pattern;
        WhenClauseSyntax* whenClause;
        SyntaxToken colonToken;

        CasePatternSwitchLabelSyntax(SyntaxToken keyword, PatternSyntax* pattern, WhenClauseSyntax* whenClause, SyntaxToken colonToken)
            : SwitchLabelSyntax(SyntaxKind::CasePatternSwitchLabel)
            , keyword(keyword)
            , pattern(pattern)
            , whenClause(whenClause)
            , colonToken(colonToken) {}

    };

    struct SwitchSectionSyntax : SyntaxBase {

        SyntaxList<SwitchLabelSyntax>* labels;
        SyntaxList<StatementSyntax>* statements;

        SwitchSectionSyntax(SyntaxList<SwitchLabelSyntax>* labels, SyntaxList<StatementSyntax>* statements)
            : SyntaxBase(SyntaxKind::SwitchSection)
            , labels(labels)
            , statements(statements) {
            // min size = 1
            assert(labels->size >= 1);
            assert(statements->size >= 1);
        }

    };

    struct SwitchStatementSyntax : StatementSyntax {

        SyntaxToken switchKeyword;
        SyntaxToken openParenToken;
        ExpressionSyntax* expression;
        SyntaxToken closeParenToken;
        SyntaxToken openBraceToken;
        SyntaxList<SwitchSectionSyntax>* sections;
        SyntaxToken closeBraceToken;

        SwitchStatementSyntax(SyntaxToken switchKeyword, SyntaxToken openParenToken, ExpressionSyntax* expression, SyntaxToken closeParenToken, SyntaxToken openBraceToken, SyntaxList<SwitchSectionSyntax>* sections, SyntaxToken closeBraceToken)
            : StatementSyntax(SyntaxKind::SwitchStatement)
            , switchKeyword(switchKeyword)
            , openParenToken(openParenToken)
            , expression(expression)
            , closeParenToken(closeParenToken)
            , openBraceToken(openBraceToken)
            , sections(sections)
            , closeBraceToken(closeBraceToken) {}

    };

    struct UsingStatementSyntax : StatementSyntax {

        SyntaxToken usingKeyword;
        SyntaxToken openParenToken;
        VariableDeclarationSyntax* declaration;
        ExpressionSyntax* expression;
        SyntaxToken closeParenToken;
        StatementSyntax* statement;

        UsingStatementSyntax(SyntaxToken usingKeyword, SyntaxToken openParenToken, VariableDeclarationSyntax* declaration, ExpressionSyntax* expression, SyntaxToken closeParenToken, StatementSyntax* statement)
            : StatementSyntax(SyntaxKind::UsingStatement)
            , usingKeyword(usingKeyword)
            , openParenToken(openParenToken)
            , declaration(declaration)
            , expression(expression)
            , closeParenToken(closeParenToken)
            , statement(statement) {}
    };

    struct WhileStatementSyntax : StatementSyntax {

        SyntaxToken whileKeyword;
        SyntaxToken openParen;
        ExpressionSyntax* condition;
        SyntaxToken closeParen;
        StatementSyntax* statement;

        WhileStatementSyntax(SyntaxToken whileKeyword, SyntaxToken openParen, ExpressionSyntax* condition, SyntaxToken closeParen, StatementSyntax* statement)
            : StatementSyntax(SyntaxKind::WhileStatement)
            , whileKeyword(whileKeyword)
            , openParen(openParen)
            , condition(condition)
            , closeParen(closeParen)
            , statement(statement) {}

    };

    struct DoStatementSyntax : StatementSyntax {

        SyntaxToken doKeyword;
        StatementSyntax* statement;
        SyntaxToken whileKeyword;
        SyntaxToken openParen;
        ExpressionSyntax* condition;
        SyntaxToken closeParen;
        SyntaxToken semicolon;

        DoStatementSyntax(SyntaxToken doKeyword, StatementSyntax* statement, SyntaxToken whileKeyword, SyntaxToken openParen, ExpressionSyntax* condition, SyntaxToken closeParen, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::DoStatement)
            , doKeyword(doKeyword)
            , statement(statement)
            , whileKeyword(whileKeyword)
            , openParen(openParen)
            , condition(condition)
            , closeParen(closeParen)
            , semicolon(semicolon) {}

    };

    struct ArrayRankSpecifierSyntax : ExpressionSyntax {
        SyntaxToken open;
        SeparatedSyntaxList<ExpressionSyntax>* ranks;
        SyntaxToken close;

        ArrayRankSpecifierSyntax(SyntaxToken open, SeparatedSyntaxList<ExpressionSyntax>* ranks, SyntaxToken close)
            : ExpressionSyntax(SyntaxKind::ArrayRankSpecifier)
            , open(open)
            , ranks(ranks)
            , close(close) {}

    };

    struct TypeArgumentListSyntax : SyntaxBase {

        SyntaxToken lessThanToken;
        SeparatedSyntaxList<TypeSyntax>* arguments;
        SyntaxToken greaterThanToken;

        TypeArgumentListSyntax(SyntaxToken lessThanToken, SeparatedSyntaxList<TypeSyntax>* arguments, SyntaxToken greaterThanToken)
            : SyntaxBase(SyntaxKind::TypeArgumentList)
            , lessThanToken(lessThanToken)
            , arguments(arguments)
            , greaterThanToken(greaterThanToken) {}

    };

    struct GenericNameSyntax : SimpleNameSyntax {

        SyntaxToken identifier;
        TypeArgumentListSyntax* typeArgumentList;

        GenericNameSyntax(SyntaxToken identifier, TypeArgumentListSyntax* typeArgumentListSyntax)
            : SimpleNameSyntax(SyntaxKind::GenericName)
            , identifier(identifier)
            , typeArgumentList(typeArgumentListSyntax) {}

    };

    struct BracketedArgumentListSyntax;

    struct ElementBindingExpressionSyntax : ExpressionSyntax {

        BracketedArgumentListSyntax* argumentList;

        explicit ElementBindingExpressionSyntax(BracketedArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::ElementBindingExpression)
            , argumentList(argumentList) {}

    };

    struct MemberBindingExpressionSyntax : ExpressionSyntax {

        SyntaxToken operatorToken;
        SimpleNameSyntax* name;

        MemberBindingExpressionSyntax(SyntaxToken operatorToken, SimpleNameSyntax* name)
            : ExpressionSyntax(SyntaxKind::MemberBindingExpression)
            , operatorToken(operatorToken)
            , name(name) {}

    };

    struct ConditionalAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;
        ExpressionSyntax* whenNotNull;

        ConditionalAccessExpressionSyntax(ExpressionSyntax* expression, SyntaxToken operatorToken, ExpressionSyntax* whenNotNull)
            : ExpressionSyntax(SyntaxKind::ConditionalAccessExpression)
            , expression(expression)
            , operatorToken(operatorToken)
            , whenNotNull(whenNotNull) {}

    };

    abstract struct MemberAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;
        SimpleNameSyntax* name;

        MemberAccessExpressionSyntax(SyntaxKind kind, ExpressionSyntax* expression, SyntaxToken operatorToken, SimpleNameSyntax* name)
            : ExpressionSyntax(kind)
            , expression(expression)
            , operatorToken(operatorToken)
            , name(name) {}

    };

    struct QualifiedNameSyntax : NameSyntax {
        NameSyntax* left;
        SyntaxToken dotToken;
        SimpleNameSyntax* right;

        explicit QualifiedNameSyntax(NameSyntax* left, SyntaxToken dotToken, SimpleNameSyntax* right)
            : NameSyntax(SyntaxKind::QualifiedName)
            , left(left)
            , dotToken(dotToken)
            , right(right) {}

    };

    struct IdentifierNameSyntax : SimpleNameSyntax {

        SyntaxToken identifier;

        explicit IdentifierNameSyntax(SyntaxToken identifier)
            : SimpleNameSyntax(SyntaxKind::IdentifierName)
            , identifier(identifier) {}

    };

    struct NameColonSyntax : BaseExpressionColonSyntax {
        IdentifierNameSyntax* name;
        SyntaxToken colonToken;

        NameColonSyntax(IdentifierNameSyntax* name, SyntaxToken colonToken)
            : BaseExpressionColonSyntax(SyntaxKind::NameColon)
            , name(name)
            , colonToken(colonToken) {}

    };

    struct PredefinedTypeSyntax : TypeSyntax {
        SyntaxToken typeToken;

        explicit PredefinedTypeSyntax(SyntaxToken typeToken)
            : TypeSyntax(SyntaxKind::PredefinedType)
            , typeToken(typeToken) {}

    };

    struct TupleElementSyntax : SyntaxBase {
        TypeSyntax* type;
        SyntaxToken identifier; // optional

        TupleElementSyntax(TypeSyntax* type, SyntaxToken identifier)
            : SyntaxBase(SyntaxKind::TupleElement)
            , type(type)
            , identifier(identifier) {}

    };

    struct TupleTypeSyntax : TypeSyntax {

        SyntaxToken openParenToken;
        SeparatedSyntaxList<TupleElementSyntax>* elements;
        SyntaxToken closeParenToken;

        TupleTypeSyntax(SyntaxToken openParenToken, SeparatedSyntaxList<TupleElementSyntax>* elements, SyntaxToken closeParenToken)
            : TypeSyntax(SyntaxKind::TupleType)
            , openParenToken(openParenToken)
            , elements(elements)
            , closeParenToken(closeParenToken) {}

    };

    struct ArrayTypeSyntax : TypeSyntax {

        TypeSyntax* elementType;
        SyntaxList<ArrayRankSpecifierSyntax>* ranks;

        ArrayTypeSyntax(TypeSyntax* elementType, SyntaxList<ArrayRankSpecifierSyntax>* ranks)
            : TypeSyntax(SyntaxKind::ArrayType)
            , elementType(elementType)
            , ranks(ranks) {}

    };

    struct RefTypeSyntax : TypeSyntax {

        SyntaxToken refKeyword;
        SyntaxToken readonlyKeyword;
        TypeSyntax* type;

        RefTypeSyntax(SyntaxToken refKeyword, SyntaxToken readonlyKeyword, TypeSyntax* typeSyntax)
            : TypeSyntax(SyntaxKind::RefType)
            , refKeyword(refKeyword)
            , readonlyKeyword(readonlyKeyword)
            , type(typeSyntax) {}

    };

    struct LabeledStatementSyntax : StatementSyntax {

        SyntaxToken identifier;
        SyntaxToken colon;
        StatementSyntax* statement;

        LabeledStatementSyntax(SyntaxToken identifier, SyntaxToken colon, StatementSyntax* statement)
            : StatementSyntax(SyntaxKind::LabeledStatement)
            , identifier(identifier)
            , colon(colon)
            , statement(statement) {}

    };

    struct OmittedArraySizeExpressionSyntax : ExpressionSyntax {

        SyntaxToken token;

        explicit OmittedArraySizeExpressionSyntax(SyntaxToken token)
            : ExpressionSyntax(SyntaxKind::OmittedArraySizeExpression)
            , token(token) {}

    };


    struct NullableTypeSyntax : TypeSyntax {

        TypeSyntax* elementType;
        SyntaxToken questionMark;

        NullableTypeSyntax(TypeSyntax* elementType, SyntaxToken questionMark)
            : TypeSyntax(SyntaxKind::NullableType)
            , elementType(elementType)
            , questionMark(questionMark) {}

    };

    struct ArgumentSyntax : SyntaxBase {

        NameColonSyntax* nameColon;
        SyntaxToken refKindKeyword;
        ExpressionSyntax* expression;

        ArgumentSyntax(NameColonSyntax* nameColon, SyntaxToken refKindKeyword, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::Argument)
            , nameColon(nameColon)
            , refKindKeyword(refKindKeyword)
            , expression(expression) {}

    };

    struct NameEqualsSyntax : SyntaxBase {
        IdentifierNameSyntax* name;
        SyntaxToken equalsToken;

        NameEqualsSyntax(IdentifierNameSyntax* name, SyntaxToken equalsToken)
            : SyntaxBase(SyntaxKind::NameEquals)
            , name(name)
            , equalsToken(equalsToken) {}

    };

    struct ImplicitArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        SyntaxToken openBracket;
        TokenList* commas;
        SyntaxToken closeBracket;
        ExpressionSyntax* initializer;

        ImplicitArrayCreationExpressionSyntax(SyntaxToken newKeyword, SyntaxToken openBracket, TokenList* commas, SyntaxToken closeBracket, ExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitArrayCreationExpression)
            , newKeyword(newKeyword)
            , openBracket(openBracket)
            , commas(commas)
            , closeBracket(closeBracket)
            , initializer(initializer) {}


    };


    struct InitializerExpressionSyntax : ExpressionSyntax {

        SyntaxToken openBraceToken;
        SeparatedSyntaxList<ExpressionSyntax>* list;
        SyntaxToken closeBraceToken;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::ObjectInitializerExpression,
            SyntaxKind::CollectionInitializerExpression,
            SyntaxKind::ArrayInitializerExpression,
            SyntaxKind::ComplexElementInitializerExpression,
            SyntaxKind::WithInitializerExpression,
        };

        InitializerExpressionSyntax(SyntaxKind kind, SyntaxToken openBraceToken, SeparatedSyntaxList<ExpressionSyntax>* list, SyntaxToken closeBraceToken)
            : ExpressionSyntax(kind)
            , openBraceToken(openBraceToken)
            , list(list)
            , closeBraceToken(closeBraceToken) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct StackAllocArrayCreationExpressionSyntax : ExpressionSyntax {

        SyntaxToken stackallocKeyword;
        TypeSyntax* type;
        InitializerExpressionSyntax* initializer;

        StackAllocArrayCreationExpressionSyntax(SyntaxToken stackallocKeyword, TypeSyntax* type, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::StackAllocArrayCreationExpression)
            , stackallocKeyword(stackallocKeyword)
            , type(type)
            , initializer(initializer) {}

    };

    struct ImplicitStackAllocArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken stackallocKeyword;
        SyntaxToken openBracket;
        SyntaxToken closeBracket;
        InitializerExpressionSyntax* initializer;

        ImplicitStackAllocArrayCreationExpressionSyntax(SyntaxToken stackallocKeyword, SyntaxToken openBracket, SyntaxToken closeBracket, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitStackAllocArrayCreationExpression)
            , stackallocKeyword(stackallocKeyword)
            , openBracket(openBracket)
            , closeBracket(closeBracket)
            , initializer(initializer) {}

    };

    struct ArgumentListSyntax : SyntaxBase {
        SyntaxToken openToken;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeToken;

        ArgumentListSyntax(SyntaxToken openToken, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeToken)
            : SyntaxBase(SyntaxKind::ArgumentList)
            , openToken(openToken)
            , arguments(arguments)
            , closeToken(closeToken) {}

    };

    struct ObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        TypeSyntax* type;
        ArgumentListSyntax* arguments;
        InitializerExpressionSyntax* initializer;

        ObjectCreationExpressionSyntax(SyntaxToken newKeyword, TypeSyntax* type, ArgumentListSyntax* arguments, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ObjectCreationExpression)
            , newKeyword(newKeyword)
            , type(type)
            , arguments(arguments)
            , initializer(initializer) {}

    };

    struct ImplicitObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        ArgumentListSyntax* arguments;
        InitializerExpressionSyntax* initializer;

        ImplicitObjectCreationExpressionSyntax(SyntaxToken newKeyword, ArgumentListSyntax* arguments, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ImplicitObjectCreationExpression)
            , newKeyword(newKeyword)
            , arguments(arguments)
            , initializer(initializer) {}

    };


    struct ArrayCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newKeyword;
        ArrayTypeSyntax* type;
        InitializerExpressionSyntax* initializer;

        ArrayCreationExpressionSyntax(SyntaxToken newKeyword, ArrayTypeSyntax* type, InitializerExpressionSyntax* initializer)
            : ExpressionSyntax(SyntaxKind::ArrayCreationExpression)
            , newKeyword(newKeyword)
            , type(type)
            , initializer(initializer) {}
    };

    struct AnonymousObjectMemberDeclaratorSyntax : ExpressionSyntax {
        NameEqualsSyntax* nameEquals;
        ExpressionSyntax* expression;

        AnonymousObjectMemberDeclaratorSyntax(NameEqualsSyntax* nameEquals, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::AnonymousObjectMemberDeclarator)
            , nameEquals(nameEquals)
            , expression(expression) {}

    };

    struct AnonymousObjectCreationExpressionSyntax : ExpressionSyntax {
        SyntaxToken newToken;
        SyntaxToken openBrace;
        SeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>* initializers;
        SyntaxToken closeBrace;

        AnonymousObjectCreationExpressionSyntax(SyntaxToken newToken, SyntaxToken openBrace, SeparatedSyntaxList<AnonymousObjectMemberDeclaratorSyntax>* initializers, SyntaxToken closeBrace)
            : ExpressionSyntax(SyntaxKind::AnonymousObjectCreationExpression)
            , newToken(newToken)
            , openBrace(openBrace)
            , initializers(initializers)
            , closeBrace(closeBrace) {}

    };

    struct TupleExpressionSyntax : ExpressionSyntax {
        SyntaxToken openToken;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeToken;

        TupleExpressionSyntax(SyntaxToken openToken, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeToken)
            : ExpressionSyntax(SyntaxKind::TupleExpression)
            , openToken(openToken)
            , arguments(arguments)
            , closeToken(closeToken) {}

    };

    struct ParenthesizedExpressionSyntax : ExpressionSyntax {
        SyntaxToken openToken;
        ExpressionSyntax* expression;
        SyntaxToken closeToken;

        ParenthesizedExpressionSyntax(SyntaxToken openToken, ExpressionSyntax* expression, SyntaxToken closeToken)
            : ExpressionSyntax(SyntaxKind::ParenthesizedExpression)
            , openToken(openToken)
            , expression(expression)
            , closeToken(closeToken) {}

    };


    struct BracketedArgumentListSyntax : SyntaxBase {

        SyntaxToken openBracket;
        SeparatedSyntaxList<ArgumentSyntax>* arguments;
        SyntaxToken closeBracket;

        BracketedArgumentListSyntax(SyntaxToken openBracket, SeparatedSyntaxList<ArgumentSyntax>* arguments, SyntaxToken closeBracket)
            : SyntaxBase(SyntaxKind::BracketedArgumentList)
            , openBracket(openBracket)
            , arguments(arguments)
            , closeBracket(closeBracket) {}

    };

    struct EqualsValueClauseSyntax : SyntaxBase {

        SyntaxToken equalsToken;
        ExpressionSyntax* value;

        EqualsValueClauseSyntax(SyntaxToken equalsToken, ExpressionSyntax* value)
            : SyntaxBase(SyntaxKind::EqualsValueClause)
            , equalsToken(equalsToken)
            , value(value) {}

    };

    struct RefExpressionSyntax : ExpressionSyntax {

        SyntaxToken refKeyword;
        ExpressionSyntax* expression;

        RefExpressionSyntax(SyntaxToken refKeyword, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::RefExpression)
            , refKeyword(refKeyword)
            , expression(expression) {}

    };

    struct VariableDeclaratorSyntax : SyntaxBase {

        SyntaxToken identifier;
        BracketedArgumentListSyntax* argumentList;
        EqualsValueClauseSyntax* initializer;

        explicit VariableDeclaratorSyntax(SyntaxToken identifier, BracketedArgumentListSyntax* argumentList, EqualsValueClauseSyntax* initializer)
            : SyntaxBase(SyntaxKind::VariableDeclarator)
            , identifier(identifier)
            , argumentList(argumentList)
            , initializer(initializer) {}

    };

    struct TypeParameterSyntax : SyntaxBase {

        SyntaxToken identifier;

        explicit TypeParameterSyntax(SyntaxToken identifier) : SyntaxBase(SyntaxKind::TypeParameter) {}

    };

    struct TypeParameterListSyntax : SyntaxBase {

        SyntaxToken lessThanToken;
        SeparatedSyntaxList<TypeParameterSyntax>* parameters;
        SyntaxToken greaterThanToken;

        explicit TypeParameterListSyntax(SyntaxToken lessThanToken, SeparatedSyntaxList<TypeParameterSyntax>* parameters, SyntaxToken greaterThanToken)
            : SyntaxBase(SyntaxKind::TypeParameterList)
            , lessThanToken(lessThanToken)
            , parameters(parameters)
            , greaterThanToken(greaterThanToken) {}

    };

    struct ArrowExpressionClauseSyntax : SyntaxBase {
        SyntaxToken arrowToken;
        ExpressionSyntax* expression;

        ArrowExpressionClauseSyntax(SyntaxToken arrowToken, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::ArrowExpressionClause)
            , arrowToken(arrowToken)
            , expression(expression) {}

    };

    struct BlockSyntax : StatementSyntax {

        SyntaxToken openBraceToken;
        SyntaxList<StatementSyntax>* statements;
        SyntaxToken closeBraceToken;

        BlockSyntax(SyntaxToken openBraceToken, SyntaxList<StatementSyntax>* statements, SyntaxToken closeBraceToken)
            : StatementSyntax(SyntaxKind::Block)
            , openBraceToken(openBraceToken)
            , statements(statements)
            , closeBraceToken(closeBraceToken) {}

    };

    struct LiteralExpressionSyntax : ExpressionSyntax {

        SyntaxToken literal;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::CharacterLiteralExpression,
            SyntaxKind::DefaultLiteralExpression,
            SyntaxKind::FalseLiteralExpression,
            SyntaxKind::NullLiteralExpression,
            SyntaxKind::NumericLiteralExpression,
            SyntaxKind::StringLiteralExpression,
            SyntaxKind::TrueLiteralExpression,
        };

        LiteralExpressionSyntax(SyntaxKind kind, SyntaxToken literal)
            : ExpressionSyntax(kind)
            , literal(literal) {

            ASSERT_VALID_SYNTAX_KIND(kind);

        }

    };

    struct CastExpressionSyntax : ExpressionSyntax {
        SyntaxToken openParen;
        TypeSyntax* type;
        SyntaxToken closeParen;
        ExpressionSyntax* expression;

        CastExpressionSyntax(SyntaxToken openParen, TypeSyntax* type, SyntaxToken closeParen, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::CastExpression)
            , openParen(openParen)
            , type(type)
            , closeParen(closeParen)
            , expression(expression) {}

    };

    struct BaseExpressionSyntax : ExpressionSyntax {

        SyntaxToken keyword;

        explicit BaseExpressionSyntax(SyntaxToken keyword)
            : ExpressionSyntax(SyntaxKind::BaseExpression)
            , keyword(keyword) {}

    };

    struct ThisExpressionSyntax : ExpressionSyntax {

        SyntaxToken keyword;

        explicit ThisExpressionSyntax(SyntaxToken keyword)
            : ExpressionSyntax(SyntaxKind::ThisExpression)
            , keyword(keyword) {}

    };

    struct DefaultExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        DefaultExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::DefaultExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct SizeOfExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        SizeOfExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::SizeOfExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct TypeOfExpressionSyntax : ExpressionSyntax {
        SyntaxToken keyword;
        SyntaxToken openParenToken;
        TypeSyntax* type;
        SyntaxToken closeParenToken;

        TypeOfExpressionSyntax(SyntaxToken keyword, SyntaxToken openParenToken, TypeSyntax* type, SyntaxToken closeParenToken)
            : ExpressionSyntax(SyntaxKind::TypeOfExpression)
            , keyword(keyword)
            , openParenToken(openParenToken)
            , type(type)
            , closeParenToken(closeParenToken) {}

    };

    struct DiscardDesignationSyntax : VariableDesignationSyntax {
        SyntaxToken underscoreToken;

        explicit DiscardDesignationSyntax(SyntaxToken underscoreToken)
            : VariableDesignationSyntax(SyntaxKind::DiscardDesignation)
            , underscoreToken(underscoreToken) {}

    };

    struct SingleVariableDesignationSyntax : VariableDesignationSyntax {
        SyntaxToken identifier;

        explicit SingleVariableDesignationSyntax(SyntaxToken identifier)
            : VariableDesignationSyntax(SyntaxKind::SingleVariableDesignation)
            , identifier(identifier) {}

    };

    struct ParenthesizedVariableDesignationSyntax : VariableDesignationSyntax {

        SyntaxToken openParen;
        SeparatedSyntaxList<VariableDesignationSyntax>* designators;
        SyntaxToken closeParen;

        ParenthesizedVariableDesignationSyntax(SyntaxToken openParen, SeparatedSyntaxList<VariableDesignationSyntax>* designators, SyntaxToken closeParen)
            : VariableDesignationSyntax(SyntaxKind::ParenthesizedVariableDesignation)
            , openParen(openParen)
            , designators(designators)
            , closeParen(closeParen) {}

    };

    struct ExpressionElementSyntax : CollectionElementSyntax {

        ExpressionSyntax* expression;

        explicit ExpressionElementSyntax(ExpressionSyntax* expression)
            : CollectionElementSyntax(SyntaxKind::ExpressionElement)
            , expression(expression) {}

    };

    struct SpreadElementSyntax : CollectionElementSyntax {

        SyntaxToken dotDotToken;
        ExpressionSyntax* expression;

        SpreadElementSyntax(SyntaxToken dotDotToken, ExpressionSyntax* expression)
            : CollectionElementSyntax(SyntaxKind::SpreadElement)
            , dotDotToken(dotDotToken)
            , expression(expression) {}

    };

    struct CollectionExpressionSyntax : ExpressionSyntax {
        SyntaxToken open;
        SeparatedSyntaxList<CollectionElementSyntax>* elements;
        SyntaxToken close;

        explicit CollectionExpressionSyntax(SyntaxToken open, SeparatedSyntaxList<CollectionElementSyntax>* elements, SyntaxToken close)
            : ExpressionSyntax(SyntaxKind::CollectionExpression)
            , open(open)
            , elements(elements)
            , close(close) {}

    };

    struct DeclarationExpressionSyntax : ExpressionSyntax {

        TypeSyntax* type;
        VariableDesignationSyntax* designation;

        DeclarationExpressionSyntax(TypeSyntax* type, VariableDesignationSyntax* designation)
            : ExpressionSyntax(SyntaxKind::DeclarationExpression)
            , type(type)
            , designation(designation) {}
    };

    struct ThrowExpressionSyntax : ExpressionSyntax {

        SyntaxToken throwKeyword;
        ExpressionSyntax* expression;

        ThrowExpressionSyntax(SyntaxToken throwKeyword, ExpressionSyntax* expression)
            : ExpressionSyntax(SyntaxKind::ThrowExpression)
            , throwKeyword(throwKeyword)
            , expression(expression) {}

    };

    struct PostfixUnaryExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        SyntaxToken operatorToken;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::PostIncrementExpression,
            SyntaxKind::PostDecrementExpression,
        };

        PostfixUnaryExpressionSyntax(SyntaxKind kind, ExpressionSyntax* expression, SyntaxToken operatorToken)
            : ExpressionSyntax(kind)
            , expression(expression)
            , operatorToken(operatorToken) {

            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct ElementAccessExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        BracketedArgumentListSyntax* argumentList;

        ElementAccessExpressionSyntax(ExpressionSyntax* expression, BracketedArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::ElementAccessExpression)
            , expression(expression)
            , argumentList(argumentList) {}
    };

    struct InvocationExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* expression;
        ArgumentListSyntax* argumentList;

        InvocationExpressionSyntax(ExpressionSyntax* expression, ArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::InvocationExpression)
            , expression(expression)
            , argumentList(argumentList) {}

    };

    struct ConditionalExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* condition;
        SyntaxToken questionToken;
        ExpressionSyntax* whenTrue;
        SyntaxToken colonToken;
        ExpressionSyntax* whenFalse;

        ConditionalExpressionSyntax(ExpressionSyntax* condition, SyntaxToken questionToken, ExpressionSyntax* whenTrue, SyntaxToken colonToken, ExpressionSyntax* whenFalse)
            : ExpressionSyntax(SyntaxKind::ConditionalExpression)
            , condition(condition)
            , questionToken(questionToken)
            , whenTrue(whenTrue)
            , colonToken(colonToken)
            , whenFalse(whenFalse) {}

    };

    struct RangeExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* leftOperand;
        SyntaxToken operatorToken;
        ExpressionSyntax* rightOperand;

        RangeExpressionSyntax(ExpressionSyntax* leftOperand, SyntaxToken operatorToken, ExpressionSyntax* rightOperand)
            : ExpressionSyntax(SyntaxKind::RangeExpression)
            , leftOperand(leftOperand)
            , operatorToken(operatorToken)
            , rightOperand(rightOperand) {}

    };

    struct PrefixUnaryExpressionSyntax : ExpressionSyntax {

        SyntaxToken operatorToken;
        ExpressionSyntax* operand;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::UnaryPlusExpression,
            SyntaxKind::UnaryMinusExpression,
            SyntaxKind::BitwiseNotExpression,
            SyntaxKind::LogicalNotExpression,
            SyntaxKind::PreIncrementExpression,
            SyntaxKind::PreDecrementExpression,
            SyntaxKind::AddressOfExpression,
            SyntaxKind::PointerIndirectionExpression,
            SyntaxKind::IndexExpression,
        };

        PrefixUnaryExpressionSyntax(SyntaxKind opKind, SyntaxToken operatorToken, ExpressionSyntax* operand)
            : ExpressionSyntax(opKind)
            , operatorToken(operatorToken)
            , operand(operand) {

            ASSERT_VALID_SYNTAX_KIND(opKind);

        }

    };

    struct ParameterSyntax : SyntaxBase {

        TokenList* modifiers;
        TypeSyntax* type;
        SyntaxToken identifier;
        EqualsValueClauseSyntax* defaultValue;

        ParameterSyntax(TokenList* modifiers, TypeSyntax* type, SyntaxToken identifier, EqualsValueClauseSyntax* defaultValue)
            : SyntaxBase(SyntaxKind::Parameter)
            , modifiers(modifiers)
            , type(type)
            , identifier(identifier)
            , defaultValue(defaultValue) {}

    };

    struct SimpleLambdaExpressionSyntax : LambdaExpressionSyntax {

        TokenList* modifiers;
        ParameterSyntax* parameter;
        SyntaxToken arrowToken;
        BlockSyntax* blockBody; // either this or expression is non null
        ExpressionSyntax* expressionBody;

        SimpleLambdaExpressionSyntax(TokenList* modifiers, ParameterSyntax* parameter, SyntaxToken arrowToken, BlockSyntax* blockBody, ExpressionSyntax* expressionBody)
            : LambdaExpressionSyntax(SyntaxKind::SimpleLambdaExpression)
            , modifiers(modifiers)
            , parameter(parameter)
            , arrowToken(arrowToken)
            , blockBody(blockBody)
            , expressionBody(expressionBody) {}

    };

    struct ParenthesizedLambdaExpressionSyntax : LambdaExpressionSyntax {

        TokenList* modifiers;
        TypeSyntax* returnType;
        ParameterListSyntax* parameters;
        SyntaxToken arrowToken;
        BlockSyntax* blockBody; // either this or expression is non null
        ExpressionSyntax* expressionBody;

        ParenthesizedLambdaExpressionSyntax(TokenList* modifiers, TypeSyntax* returnType, ParameterListSyntax* parameters, SyntaxToken arrowToken, BlockSyntax* blockBody, ExpressionSyntax* expressionBody)
            : LambdaExpressionSyntax(SyntaxKind::ParenthesizedLambdaExpression)
            , modifiers(modifiers)
            , returnType(returnType)
            , parameters(parameters)
            , arrowToken(arrowToken)
            , blockBody(blockBody)
            , expressionBody(expressionBody) {}

    };

    struct BaseConstructorInitializerSyntax : ConstructorInitializerSyntax {
        SyntaxToken colonToken;
        SyntaxToken baseKeyword;
        ArgumentListSyntax* argumentListSyntax;

        BaseConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken baseKeyword, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::BaseConstructorInitializer)
            , colonToken(colonToken)
            , baseKeyword(baseKeyword)
            , argumentListSyntax(argumentListSyntax) {}

    };

    struct ThisConstructorInitializerSyntax : ConstructorInitializerSyntax {

        SyntaxToken colonToken;
        SyntaxToken thisKeyword;
        ArgumentListSyntax* argumentListSyntax;

        ThisConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken thisKeyword, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::ThisConstructorInitializer)
            , colonToken(colonToken)
            , thisKeyword(thisKeyword)
            , argumentListSyntax(argumentListSyntax) {}

    };


    struct NamedConstructorInitializerSyntax : ConstructorInitializerSyntax {

        SyntaxToken colonToken;
        SyntaxToken name;
        ArgumentListSyntax* argumentListSyntax;

        NamedConstructorInitializerSyntax(SyntaxToken colonToken, SyntaxToken name, ArgumentListSyntax* argumentListSyntax)
            : ConstructorInitializerSyntax(SyntaxKind::NamedConstructorInitializer)
            , colonToken(colonToken)
            , name(name)
            , argumentListSyntax(argumentListSyntax) {}

    };

    struct ParameterListSyntax : SyntaxBase {

        SyntaxToken openParen;
        SeparatedSyntaxList<ParameterSyntax>* parameters;
        SyntaxToken closeParen;

        explicit ParameterListSyntax(SyntaxToken openParen, SeparatedSyntaxList<ParameterSyntax>* parameters, SyntaxToken closeParen)
            : SyntaxBase(SyntaxKind::ParameterList)
            , openParen(openParen)
            , parameters(parameters)
            , closeParen(closeParen) {}

    };

    struct TypeParameterConstraintClauseSyntax;

    struct LocalFunctionStatementSyntax : StatementSyntax {

        TokenList* modifiers;
        TypeSyntax* returnType;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameters;
        ParameterListSyntax* parameters;
        SyntaxList <TypeParameterConstraintClauseSyntax>* constraints;
        BlockSyntax* blockBody;
        ArrowExpressionClauseSyntax* arrowBody;
        SyntaxToken semicolon;

        LocalFunctionStatementSyntax(TokenList* modifiers, TypeSyntax* returnType, SyntaxToken identifier, TypeParameterListSyntax* typeParameters, ParameterListSyntax* parameters, SyntaxList <TypeParameterConstraintClauseSyntax>* constraints, BlockSyntax* blockBody, ArrowExpressionClauseSyntax* arrowBody, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::LocalFunctionStatement)
            , modifiers(modifiers)
            , returnType(returnType)
            , identifier(identifier)
            , typeParameters(typeParameters)
            , parameters(parameters)
            , constraints(constraints)
            , blockBody(blockBody)
            , arrowBody(arrowBody)
            , semicolon(semicolon) {}

    };

    struct VariableDeclarationSyntax : SyntaxBase {

        TypeSyntax* type;
        SeparatedSyntaxList<VariableDeclaratorSyntax>* variables;

        explicit VariableDeclarationSyntax(TypeSyntax* type, SeparatedSyntaxList<VariableDeclaratorSyntax>* variables)
            : SyntaxBase(SyntaxKind::VariableDeclaration)
            , type(type)
            , variables(variables) {}

    };

    struct LocalDeclarationStatementSyntax : StatementSyntax {

        SyntaxToken usingKeyword; // optional
        TokenList* modifiers; // optional
        VariableDeclarationSyntax* declaration;
        SyntaxToken semicolon;

        LocalDeclarationStatementSyntax(SyntaxToken usingKeyword, TokenList* modifiers, VariableDeclarationSyntax* declaration, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::LocalDeclarationStatement)
            , usingKeyword(usingKeyword)
            , modifiers(modifiers)
            , declaration(declaration)
            , semicolon(semicolon) {}

    };

    struct FieldDeclarationSyntax : MemberDeclarationSyntax {
        TokenList* modifiers;
        VariableDeclarationSyntax* declaration;
        SyntaxToken semicolonToken;

        FieldDeclarationSyntax(TokenList* modifiers, VariableDeclarationSyntax* declaration, SyntaxToken semicolonToken)
            : MemberDeclarationSyntax(SyntaxKind::FieldDeclaration)
            , modifiers(modifiers)
            , declaration(declaration)
            , semicolonToken(semicolonToken) {}

    };

    struct ExpressionColonSyntax : BaseExpressionColonSyntax {
        ExpressionSyntax* expression;
        SyntaxToken colonToken;

        ExpressionColonSyntax(ExpressionSyntax* expression, SyntaxToken colonToken)
            : BaseExpressionColonSyntax(SyntaxKind::ExpressionColon)
            , expression(expression)
            , colonToken(colonToken) {}
    };

    struct SubpatternSyntax : PatternSyntax {

        BaseExpressionColonSyntax* expressionColon;
        PatternSyntax* pattern;

        SubpatternSyntax(BaseExpressionColonSyntax* expressionColon, PatternSyntax* pattern)
            : PatternSyntax(SyntaxKind::Subpattern)
            , expressionColon(expressionColon)
            , pattern(pattern) {}

    };

    struct PropertyPatternClauseSyntax : SyntaxBase {

        SyntaxToken openBraceToken;
        SeparatedSyntaxList<SubpatternSyntax>* subpatterns;
        SyntaxToken closeBraceToken;

        PropertyPatternClauseSyntax(SyntaxToken openBraceToken, SeparatedSyntaxList<SubpatternSyntax>* subpatterns, SyntaxToken closeBraceToken)
            : SyntaxBase(SyntaxKind::PropertyPatternClause)
            , openBraceToken(openBraceToken)
            , subpatterns(subpatterns)
            , closeBraceToken(closeBraceToken) {}

    };

    struct DeclarationPatternSyntax : PatternSyntax {
        TypeSyntax* type;
        VariableDesignationSyntax* designation;

        DeclarationPatternSyntax(TypeSyntax* type, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::DeclarationPattern)
            , type(type)
            , designation(designation) {}

    };

    struct PositionalPatternClauseSyntax : PatternSyntax {
        SyntaxToken openParenToken;
        SeparatedSyntaxList<SubpatternSyntax>* subpatterns;
        SyntaxToken closeParenToken;

        PositionalPatternClauseSyntax(SyntaxToken openParenToken, SeparatedSyntaxList<SubpatternSyntax>* subpatterns, SyntaxToken closeParenToken)
            : PatternSyntax(SyntaxKind::PositionalPatternClause)
            , openParenToken(openParenToken)
            , subpatterns(subpatterns)
            , closeParenToken(closeParenToken) {}

    };

    struct RecursivePatternSyntax : PatternSyntax {
        TypeSyntax* type;
        PositionalPatternClauseSyntax* positionalPatternClause;
        PropertyPatternClauseSyntax* propertyPatternClause;
        VariableDesignationSyntax* designation;

        RecursivePatternSyntax(TypeSyntax* type, PositionalPatternClauseSyntax* positionalPatternClause, PropertyPatternClauseSyntax* propertyPatternClause, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::RecursivePattern)
            , type(type)
            , positionalPatternClause(positionalPatternClause)
            , propertyPatternClause(propertyPatternClause)
            , designation(designation) {}
    };

    struct ParenthesizedPatternSyntax : PatternSyntax {
        SyntaxToken openParenToken;
        PatternSyntax* pattern;
        SyntaxToken closeParenToken;

        ParenthesizedPatternSyntax(SyntaxToken openParenToken, PatternSyntax* pattern, SyntaxToken closeParenToken)
            : PatternSyntax(SyntaxKind::ParenthesizedPattern)
            , openParenToken(openParenToken)
            , pattern(pattern)
            , closeParenToken(closeParenToken) {}
    };

    struct VarPatternSyntax : PatternSyntax {
        SyntaxToken varKeyword;
        VariableDesignationSyntax* designation;

        VarPatternSyntax(SyntaxToken varKeyword, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::VarPattern)
            , varKeyword(varKeyword)
            , designation(designation) {}

    };

    struct TypePatternSyntax : PatternSyntax {

        TypeSyntax* type;

        TypePatternSyntax(TypeSyntax* type)
            : PatternSyntax(SyntaxKind::TypePattern)
            , type(type) {}

    };

    struct ConstantPatternSyntax : PatternSyntax {

        ExpressionSyntax* expression;

        explicit ConstantPatternSyntax(ExpressionSyntax* expression)
            : PatternSyntax(SyntaxKind::ConstantPattern)
            , expression(expression) {}

    };

    struct RelationalPatternSyntax : PatternSyntax {
        SyntaxToken operatorToken;
        ExpressionSyntax* expression;

        RelationalPatternSyntax(SyntaxToken operatorToken, ExpressionSyntax* expression)
            : PatternSyntax(SyntaxKind::RelationalPattern)
            , operatorToken(operatorToken)
            , expression(expression) {}

    };

    struct SlicePatternSyntax : PatternSyntax {
        SyntaxToken dotDotToken;
        PatternSyntax* pattern;

        SlicePatternSyntax(SyntaxToken dotDotToken, PatternSyntax* pattern)
            : PatternSyntax(SyntaxKind::SlicePattern)
            , dotDotToken(dotDotToken)
            , pattern(pattern) {}

    };

    struct DiscardPatternSyntax : PatternSyntax {

        SyntaxToken underscore;

        explicit DiscardPatternSyntax(SyntaxToken underscore)
            : PatternSyntax(SyntaxKind::DiscardPattern)
            , underscore(underscore) {}

    };

    struct UnaryPatternSyntax : PatternSyntax {
        SyntaxToken operatorToken;
        PatternSyntax* pattern;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::NotPattern
        };

        UnaryPatternSyntax(SyntaxKind kind, SyntaxToken operatorToken, PatternSyntax* pattern)
            : PatternSyntax(kind)
            , operatorToken(operatorToken)
            , pattern(pattern) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct BinaryPatternSyntax : PatternSyntax {
        PatternSyntax* left;
        SyntaxToken operatorToken;
        PatternSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::OrPattern,
            SyntaxKind::AndPattern,
        };

        BinaryPatternSyntax(SyntaxKind kind, PatternSyntax* left, SyntaxToken operatorToken, PatternSyntax* right)
            : PatternSyntax(kind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct IsPatternExpressionSyntax : ExpressionSyntax {
        ExpressionSyntax* leftOperand;
        SyntaxToken opToken;
        PatternSyntax* pattern;

        IsPatternExpressionSyntax(ExpressionSyntax* leftOperand, SyntaxToken opToken, PatternSyntax* pattern)
            : ExpressionSyntax(SyntaxKind::IsPatternExpression)
            , leftOperand(leftOperand)
            , opToken(opToken)
            , pattern(pattern) {}

    };

    struct BinaryExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* left;
        SyntaxToken operatorToken;
        ExpressionSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::AddExpression,
            SyntaxKind::SubtractExpression,
            SyntaxKind::MultiplyExpression,
            SyntaxKind::DivideExpression,
            SyntaxKind::ModuloExpression,
            SyntaxKind::LeftShiftExpression,
            SyntaxKind::RightShiftExpression,
            SyntaxKind::UnsignedRightShiftExpression,
            SyntaxKind::LogicalOrExpression,
            SyntaxKind::LogicalAndExpression,
            SyntaxKind::BitwiseOrExpression,
            SyntaxKind::BitwiseAndExpression,
            SyntaxKind::ExclusiveOrExpression,
            SyntaxKind::EqualsExpression,
            SyntaxKind::NotEqualsExpression,
            SyntaxKind::LessThanExpression,
            SyntaxKind::LessThanOrEqualExpression,
            SyntaxKind::GreaterThanExpression,
            SyntaxKind::GreaterThanOrEqualExpression,
            SyntaxKind::IsExpression,
            SyntaxKind::AsExpression,
            SyntaxKind::CoalesceExpression,
        };

        BinaryExpressionSyntax(SyntaxKind kind, ExpressionSyntax* left, SyntaxToken operatorToken, ExpressionSyntax* right)
            : ExpressionSyntax(kind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {

            ASSERT_VALID_SYNTAX_KIND(kind);

        }


    };

    struct ImplicitElementAccessSyntax : ExpressionSyntax {

        BracketedArgumentListSyntax* argumentList;

        explicit ImplicitElementAccessSyntax(BracketedArgumentListSyntax* argumentList)
            : ExpressionSyntax(SyntaxKind::ImplicitElementAccess)
            , argumentList(argumentList) {}

    };

    struct WhenClauseSyntax : SyntaxBase {

        SyntaxToken whenKeyword;
        ExpressionSyntax* condition;

        WhenClauseSyntax(SyntaxToken whenKeyword, ExpressionSyntax* condition)
            : SyntaxBase(SyntaxKind::WhenClause)
            , whenKeyword(whenKeyword)
            , condition(condition) {}

    };

    struct SwitchExpressionArmSyntax : SyntaxBase {

        PatternSyntax* pattern;
        WhenClauseSyntax* whenClause;
        SyntaxToken equalsGreaterThanToken;
        ExpressionSyntax* expression;

        SwitchExpressionArmSyntax(PatternSyntax* pattern, WhenClauseSyntax* whenClause, SyntaxToken equalsGreaterThanToken, ExpressionSyntax* expression)
            : SyntaxBase(SyntaxKind::SwitchExpressionArm)
            , pattern(pattern)
            , whenClause(whenClause)
            , equalsGreaterThanToken(equalsGreaterThanToken)
            , expression(expression) {}

    };

    struct SwitchExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* governingExpression;
        SyntaxToken switchKeyword;
        SyntaxToken openBraceToken;
        SeparatedSyntaxList<SwitchExpressionArmSyntax>* arms;
        SyntaxToken closeBraceToken;

        SwitchExpressionSyntax(ExpressionSyntax* governingExpression, SyntaxToken switchKeyword, SyntaxToken openBraceToken, SeparatedSyntaxList<SwitchExpressionArmSyntax>* arms, SyntaxToken closeBraceToken)
            : ExpressionSyntax(SyntaxKind::SwitchExpression)
            , governingExpression(governingExpression)
            , switchKeyword(switchKeyword)
            , openBraceToken(openBraceToken)
            , arms(arms)
            , closeBraceToken(closeBraceToken) {}


    };

    struct ListPatternSyntax : PatternSyntax {

        SyntaxToken openBracketToken;
        SeparatedSyntaxList<PatternSyntax>* patterns;
        SyntaxToken closeBracketToken;
        VariableDesignationSyntax* designation; // optional

        ListPatternSyntax(SyntaxToken openBracketToken, SeparatedSyntaxList<PatternSyntax>* patterns, SyntaxToken closeBracketToken, VariableDesignationSyntax* designation)
            : PatternSyntax(SyntaxKind::ListPattern)
            , openBracketToken(openBracketToken)
            , patterns(patterns)
            , closeBracketToken(closeBracketToken)
            , designation(designation) {}

    };

    struct AssignmentExpressionSyntax : ExpressionSyntax {

        ExpressionSyntax* left;
        SyntaxToken operatorToken;
        ExpressionSyntax* right;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::SimpleAssignmentExpression,
            SyntaxKind::AddAssignmentExpression,
            SyntaxKind::SubtractAssignmentExpression,
            SyntaxKind::MultiplyAssignmentExpression,
            SyntaxKind::DivideAssignmentExpression,
            SyntaxKind::ModuloAssignmentExpression,
            SyntaxKind::AndAssignmentExpression,
            SyntaxKind::ExclusiveOrAssignmentExpression,
            SyntaxKind::OrAssignmentExpression,
            SyntaxKind::LeftShiftAssignmentExpression,
            SyntaxKind::RightShiftAssignmentExpression,
            SyntaxKind::UnsignedRightShiftAssignmentExpression,
            SyntaxKind::CoalesceAssignmentExpression,
        };

        AssignmentExpressionSyntax(SyntaxKind opKind, ExpressionSyntax* left, SyntaxToken operatorToken, ExpressionSyntax* right)
            : ExpressionSyntax(opKind)
            , left(left)
            , operatorToken(operatorToken)
            , right(right) {

            ASSERT_VALID_SYNTAX_KIND(opKind);

        }

    };

    struct ForEachStatementSyntax : CommonForEachStatementSyntax {

        SyntaxToken foreachKeyword;
        SyntaxToken openParen;
        TypeSyntax* type;
        SyntaxToken identifier;
        SyntaxToken inKeyword;
        ExpressionSyntax* expression;
        SyntaxToken closeParen;
        StatementSyntax* statement;

        ForEachStatementSyntax(SyntaxToken foreachKeyword, SyntaxToken openParen, TypeSyntax* type, SyntaxToken identifier, SyntaxToken inKeyword, ExpressionSyntax* expression, SyntaxToken closeParen, StatementSyntax* statement)
            : CommonForEachStatementSyntax(SyntaxKind::ForEachStatement)
            , foreachKeyword(foreachKeyword)
            , openParen(openParen)
            , type(type)
            , identifier(identifier)
            , inKeyword(inKeyword)
            , expression(expression)
            , closeParen(closeParen)
            , statement(statement) {}

    };

    struct ForEachVariableStatementSyntax : CommonForEachStatementSyntax {

        SyntaxToken foreachKeyword;
        SyntaxToken openParen;
        ExpressionSyntax* variable;
        SyntaxToken inKeyword;
        ExpressionSyntax* expression;
        SyntaxToken closeParen;
        StatementSyntax* statement;

        ForEachVariableStatementSyntax(SyntaxToken foreachKeyword, SyntaxToken openParen, ExpressionSyntax* variable, SyntaxToken inKeyword, ExpressionSyntax* expression, SyntaxToken closeParen, StatementSyntax* statement)
            : CommonForEachStatementSyntax(SyntaxKind::ForEachVariableStatement)
            , foreachKeyword(foreachKeyword)
            , openParen(openParen)
            , variable(variable)
            , inKeyword(inKeyword)
            , expression(expression)
            , closeParen(closeParen)
            , statement(statement) {}

    };

    struct GotoStatementSyntax : StatementSyntax {

        SyntaxToken gotoToken;
        SyntaxToken caseOrDefault;
        ExpressionSyntax* arg;
        SyntaxToken semicolon;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::GotoCaseStatement,
            SyntaxKind::GotoDefaultStatement,
            SyntaxKind::GotoStatement
        };

        GotoStatementSyntax(SyntaxKind kind, SyntaxToken gotoToken, SyntaxToken caseOrDefault, ExpressionSyntax* arg, SyntaxToken semicolon)
            : StatementSyntax(kind)
            , gotoToken(gotoToken)
            , caseOrDefault(caseOrDefault)
            , arg(arg)
            , semicolon(semicolon) {}

    };

    struct ElseClauseSyntax : SyntaxBase {

        SyntaxToken elseKeyword;
        StatementSyntax* statement;

        ElseClauseSyntax(SyntaxToken elseKeyword, StatementSyntax* statement)
            : SyntaxBase(SyntaxKind::ElseClause)
            , elseKeyword(elseKeyword)
            , statement(statement) {}

    };

    struct IfStatementSyntax : StatementSyntax {

        SyntaxToken ifKeyword;
        SyntaxToken openParen;
        ExpressionSyntax* condition;
        SyntaxToken closeParen;
        StatementSyntax* statement;
        ElseClauseSyntax* elseClause;

        IfStatementSyntax(SyntaxToken ifKeyword, SyntaxToken openParen, ExpressionSyntax* condition, SyntaxToken closeParen, StatementSyntax* statement, ElseClauseSyntax* elseClause)
            : StatementSyntax(SyntaxKind::IfStatement)
            , ifKeyword(ifKeyword)
            , openParen(openParen)
            , condition(condition)
            , closeParen(closeParen)
            , statement(statement)
            , elseClause(elseClause) {}

    };

    struct ExpressionStatementSyntax : StatementSyntax {

        ExpressionSyntax* expression;
        SyntaxToken semicolon;

        ExpressionStatementSyntax(ExpressionSyntax* expression, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::ExpressionStatement)
            , expression(expression)
            , semicolon(semicolon) {}

    };

    struct ReturnStatementSyntax : StatementSyntax {

        SyntaxToken returnKeyword;
        ExpressionSyntax* expressionSyntax;
        SyntaxToken semicolon;

        ReturnStatementSyntax(SyntaxToken returnKeyword, ExpressionSyntax* expressionSyntax, SyntaxToken semicolon)
            : StatementSyntax(SyntaxKind::ReturnStatement)
            , returnKeyword(returnKeyword)
            , expressionSyntax(expressionSyntax)
            , semicolon(semicolon) {}


    };

    struct BaseListSyntax : SyntaxBase {

        SyntaxToken colonToken;
        SeparatedSyntaxList<BaseTypeSyntax>* types;

        BaseListSyntax(SyntaxToken colonToken, SeparatedSyntaxList<BaseTypeSyntax>* types)
            : SyntaxBase(SyntaxKind::BaseList)
            , colonToken(colonToken)
            , types(types) {
            assert(types->itemCount >= 1);
        }
    };

    struct AttributeSyntax : SyntaxBase {

        NameSyntax* name;
        ArgumentListSyntax* argumentList;

        AttributeSyntax(NameSyntax* name, ArgumentListSyntax* argumentList)
            : SyntaxBase(SyntaxKind::Attribute)
            , name(name)
            , argumentList(argumentList) {}

    };

    struct AttributeListSyntax : SyntaxBase {

        SyntaxToken openBracket;
        SeparatedSyntaxList<AttributeSyntax>* attributes;
        SyntaxToken closeBracket;

        AttributeListSyntax(SyntaxToken openBracket, SeparatedSyntaxList<AttributeSyntax>* attributes, SyntaxToken closeBracket)
            : SyntaxBase(SyntaxKind::AttributeList)
            , openBracket(openBracket)
            , attributes(attributes)
            , closeBracket(closeBracket) {}

    };

    struct TypeConstraintSyntax : TypeParameterConstraintSyntax {

        TypeSyntax* type;

        explicit TypeConstraintSyntax(TypeSyntax* type)
            : TypeParameterConstraintSyntax(SyntaxKind::TypeConstraint)
            , type(type) {}

    };

    struct ConstructorConstraintSyntax : TypeParameterConstraintSyntax {

        SyntaxToken newKeyword;
        SyntaxToken openParen;
        SyntaxToken closeParen;

        ConstructorConstraintSyntax(SyntaxToken newKeyword, SyntaxToken openParen, SyntaxToken closeParen)
            : TypeParameterConstraintSyntax(SyntaxKind::ConstructorConstraint)
            , newKeyword(newKeyword)
            , openParen(openParen)
            , closeParen(closeParen) {
        }

    };


    struct ClassOrStructConstraintSyntax : TypeParameterConstraintSyntax {

        SyntaxToken keyword;
        SyntaxToken questionToken;

        VALID_SYNTAX_KINDS = {
            SyntaxKind::ClassConstraint,
            SyntaxKind::StructConstraint,
        };

        ClassOrStructConstraintSyntax(SyntaxKind kind, SyntaxToken keyword, SyntaxToken questionToken)
            : TypeParameterConstraintSyntax(kind)
            , keyword(keyword)
            , questionToken(questionToken) {
            ASSERT_VALID_SYNTAX_KIND(kind);
        }

    };

    struct TypeParameterConstraintClauseSyntax : SyntaxBase {

        SyntaxToken whereKeyword;
        IdentifierNameSyntax* name;
        SyntaxToken colonToken;
        SeparatedSyntaxList<TypeParameterConstraintSyntax>* constraints;

        TypeParameterConstraintClauseSyntax(SyntaxToken whereKeyword, IdentifierNameSyntax* name, SyntaxToken colonToken, SeparatedSyntaxList<TypeParameterConstraintSyntax>* constraints)
            : SyntaxBase(SyntaxKind::TypeParameterConstraintClause)
            , whereKeyword(whereKeyword)
            , name(name)
            , colonToken(colonToken)
            , constraints(constraints) {
            assert(constraints->itemCount >= 1);
        }
    };

    struct StructDeclarationSyntax : TypeDeclarationSyntax {
        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken keyword;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameterList;
        ParameterListSyntax* parameterList;
        BaseListSyntax* baseList;
        SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses;
        SyntaxToken openBraceToken;
        SyntaxList<MemberDeclarationSyntax>* members;
        SyntaxToken closeBraceToken;
        SyntaxToken semicolonToken;

        StructDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes,
            TokenList* modifiers,
            SyntaxToken keyword,
            SyntaxToken identifier,
            TypeParameterListSyntax* typeParameterList,
            ParameterListSyntax* parameterList,
            BaseListSyntax* baseList,
            SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses,
            SyntaxToken openBraceToken,
            SyntaxList<MemberDeclarationSyntax>* members,
            SyntaxToken closeBraceToken,
            SyntaxToken semicolonToken
        )
            : TypeDeclarationSyntax(SyntaxKind::StructDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , keyword(keyword)
            , identifier(identifier)
            , typeParameterList(typeParameterList)
            , parameterList(parameterList)
            , baseList(baseList)
            , constraintClauses(constraintClauses)
            , openBraceToken(openBraceToken)
            , members(members)
            , closeBraceToken(closeBraceToken)
            , semicolonToken(semicolonToken) {}
    };

    struct EnumMemberDeclarationSyntax : MemberDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        SyntaxToken identifier;
        EqualsValueClauseSyntax* equalsValue;

        EnumMemberDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes, SyntaxToken identifier, EqualsValueClauseSyntax* equalsValue)
            : MemberDeclarationSyntax(SyntaxKind::EnumMemberDeclaration)
            , attributes(attributes)
            , identifier(identifier)
            , equalsValue(equalsValue) {}

    };

    struct EnumDeclarationSyntax : TypeDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken keyword;
        SyntaxToken identifier;
        BaseListSyntax* baseList;
        SyntaxToken openBrace;
        SeparatedSyntaxList<EnumMemberDeclarationSyntax>* members;
        SyntaxToken closeBrace;
        SyntaxToken semicolonToken;

        EnumDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes, TokenList* modifiers, SyntaxToken keyword, SyntaxToken identifier, BaseListSyntax* baseList, SyntaxToken openBrace, SeparatedSyntaxList<EnumMemberDeclarationSyntax>* members, SyntaxToken closeBrace, SyntaxToken semicolonToken)
            : TypeDeclarationSyntax(SyntaxKind::EnumDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , keyword(keyword)
            , identifier(identifier)
            , baseList(baseList)
            , openBrace(openBrace)
            , members(members)
            , closeBrace(closeBrace)
            , semicolonToken(semicolonToken) {}

    };

    struct DelegateDeclarationSyntax : TypeDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken keyword;
        TypeSyntax* returnType;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameterList;
        ParameterListSyntax* parameterList;
        SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses;
        SyntaxToken semicolonToken;

        DelegateDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes, TokenList* modifiers, SyntaxToken keyword, TypeSyntax* returnType, SyntaxToken identifier, TypeParameterListSyntax* typeParameterList, ParameterListSyntax* parameterList, SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses, SyntaxToken semicolonToken)
            : TypeDeclarationSyntax(SyntaxKind::DelegateDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , keyword(keyword)
            , returnType(returnType)
            , identifier(identifier)
            , typeParameterList(typeParameterList)
            , parameterList(parameterList)
            , constraintClauses(constraintClauses)
            , semicolonToken(semicolonToken) {}

    };

    struct ClassDeclarationSyntax : TypeDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken keyword;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameterList;
        ParameterListSyntax* parameterList;
        BaseListSyntax* baseList;
        SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses;
        SyntaxToken openBraceToken;
        SyntaxList<MemberDeclarationSyntax>* members;
        SyntaxToken closeBraceToken;
        SyntaxToken semicolonToken;

        ClassDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes,
            TokenList* modifiers,
            SyntaxToken keyword,
            SyntaxToken identifier,
            TypeParameterListSyntax* typeParameterList,
            ParameterListSyntax* parameterList,
            BaseListSyntax* baseList,
            SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses,
            SyntaxToken openBraceToken,
            SyntaxList<MemberDeclarationSyntax>* members,
            SyntaxToken closeBraceToken,
            SyntaxToken semicolonToken
        )
            : TypeDeclarationSyntax(SyntaxKind::ClassDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , keyword(keyword)
            , identifier(identifier)
            , typeParameterList(typeParameterList)
            , parameterList(parameterList)
            , baseList(baseList)
            , constraintClauses(constraintClauses)
            , openBraceToken(openBraceToken)
            , members(members)
            , closeBraceToken(closeBraceToken)
            , semicolonToken(semicolonToken) {}

    };

    struct InterfaceDeclarationSyntax : TypeDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken keyword;
        SyntaxToken identifier;
        TypeParameterListSyntax* typeParameterList;
        ParameterListSyntax* parameterList;
        BaseListSyntax* baseList;
        SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses;
        SyntaxToken openBraceToken;
        SyntaxList<MemberDeclarationSyntax>* members;
        SyntaxToken closeBraceToken;
        SyntaxToken semicolonToken;

        InterfaceDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes,
            TokenList* modifiers,
            SyntaxToken keyword,
            SyntaxToken identifier,
            TypeParameterListSyntax* typeParameterList,
            ParameterListSyntax* parameterList,
            BaseListSyntax* baseList,
            SyntaxList<TypeParameterConstraintClauseSyntax>* constraintClauses,
            SyntaxToken openBraceToken,
            SyntaxList<MemberDeclarationSyntax>* members,
            SyntaxToken closeBraceToken,
            SyntaxToken semicolonToken
        )
            : TypeDeclarationSyntax(SyntaxKind::InterfaceDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , keyword(keyword)
            , identifier(identifier)
            , typeParameterList(typeParameterList)
            , parameterList(parameterList)
            , baseList(baseList)
            , constraintClauses(constraintClauses)
            , openBraceToken(openBraceToken)
            , members(members)
            , closeBraceToken(closeBraceToken)
            , semicolonToken(semicolonToken) {}

    };

    struct ConstructorDeclarationSyntax : MemberDeclarationSyntax {

        SyntaxList<AttributeListSyntax>* attributes;
        TokenList* modifiers;
        SyntaxToken identifier;
        ParameterListSyntax* parameterList;
        ConstructorInitializerSyntax* initializer;
        BlockSyntax* bodyBlock;
        ArrowExpressionClauseSyntax* bodyExpression;
        SyntaxToken semiColon;

        ConstructorDeclarationSyntax(SyntaxList<AttributeListSyntax>* attributes, TokenList* modifiers, SyntaxToken identifier, ParameterListSyntax* parameterList, ConstructorInitializerSyntax* initializer, BlockSyntax* bodyBlock, ArrowExpressionClauseSyntax* bodyExpression, SyntaxToken semiColon)
            : MemberDeclarationSyntax(SyntaxKind::ConstructorDeclaration)
            , attributes(attributes)
            , modifiers(modifiers)
            , identifier(identifier)
            , parameterList(parameterList)
            , initializer(initializer)
            , bodyBlock(bodyBlock)
            , bodyExpression(bodyExpression)
            , semiColon(semiColon) {}

    };

    struct SimpleBaseTypeSyntax : BaseTypeSyntax {

        TypeSyntax* type;

        explicit SimpleBaseTypeSyntax(TypeSyntax* type)
            : BaseTypeSyntax(SyntaxKind::SimpleBaseType)
            , type(type) {}

    };

    struct PrimaryConstructorBaseTypeSyntax : BaseTypeSyntax {

        TypeSyntax* type;
        ArgumentListSyntax* argumentList;

        PrimaryConstructorBaseTypeSyntax(TypeSyntax* type, ArgumentListSyntax* argumentList)
            : BaseTypeSyntax(SyntaxKind::PrimaryConstructorBaseType)
            , type(type)
            , argumentList(argumentList) {}

    };


}

#undef abstract
