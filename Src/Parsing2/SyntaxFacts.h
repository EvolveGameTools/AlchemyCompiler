#pragma once

#include "./SyntaxKind.h"
#include "Precidence.h"

namespace Alchemy::Compilation::SyntaxFacts {

    bool IsHexDigit(char c);

    int32 HexValue(char c);

    bool IsBinaryDigit(char ch);

    bool IsDecDigit(char c);

    bool IsAnyUnaryExpression(TokenKind kind);

    bool IsBinaryExpression(TokenKind kind);

    bool IsAssignmentExpressionOperatorToken(TokenKind kind);

    bool IsPredefinedType(TokenKind kind);

    bool IsReservedKeyword(TokenKind kind);

    bool IsContextualKeyword(TokenKind kind);

    bool IsToken(TokenKind kind);

    const char* GetText(TokenKind kind);

    bool IsBinaryExpressionOperatorToken(TokenKind kind);

    bool IsValidArgumentRefKindKeyword(TokenKind kind);

    bool IsInvalidSubExpression(TokenKind kind);

    SyntaxKind GetPrefixUnaryExpression(TokenKind kind);

    bool IsExpectedPrefixUnaryOperator(TokenKind kind);

    Precedence GetPrecedence(SyntaxKind kind);

    SyntaxKind GetLiteralExpression(TokenKind kind);

    bool CanFollowCast(TokenKind kind);

    SyntaxKind GetBinaryExpression(TokenKind kind);

    SyntaxKind GetPostfixUnaryExpression(TokenKind token);

    SyntaxKind GetAssignmentExpression(TokenKind kind);

    bool IsRightAssociative(SyntaxKind kind);

    bool IsPatternSyntax(SyntaxKind kind);

    bool IsTypeSyntax(SyntaxKind kind);

    bool CanTokenFollowTypeInPattern(TokenKind kind, Precedence precedence);

    bool IsDeclarationModifier(TokenKind kind);

    bool IsAdditionalLocalFunctionModifier(TokenKind kind);

    bool IsTypeModifierOrTypeKeyword(TokenKind kind);

    bool IsAccessibilityModifier(TokenKind kind);

    bool IsExpressionSyntax(SyntaxKind kind);

    bool IsName(SyntaxKind kind);

    bool IsLiteralExpression(TokenKind kind);

}