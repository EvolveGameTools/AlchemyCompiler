#pragma once

#include "./SyntaxKind.h"

namespace Alchemy::Compilation::SyntaxFacts {

    bool IsHexDigit(char c);

    int32 HexValue(char c);

    bool IsBinaryDigit(char ch);

    bool IsDecDigit(char c);

    bool IsAnyUnaryExpression(SyntaxKind kind);

    bool IsBinaryExpression(SyntaxKind kind);

    bool IsAssignmentExpressionOperatorToken(SyntaxKind kind);

    bool IsPredefinedType(SyntaxKind kind);

    bool IsReservedKeyword(SyntaxKind kind);

    bool IsContextualKeyword(SyntaxKind kind);

    bool IsToken(SyntaxKind kind);

    const char* GetText(SyntaxKind kind);

}