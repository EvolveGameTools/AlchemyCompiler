#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/CheckedArray.h"
#include "./Diagnostics.h"
#include "./Precidence.h"
#include "./TerminatorState.h"

namespace Alchemy::Compilation {

    struct Parser  {

        CheckedArray<SyntaxToken> hotTokens;
        CheckedArray<SyntaxTokenCold> coldTokens;
        SyntaxToken currentToken;
        int32 ptr;
        TerminatorState _termState;
        LinearAllocator* allocator;
        TempAllocator * tempAllocator;
        Diagnostics* diagnostics;

        Parser(LinearAllocator * allocator, TempAllocator * tempAllocator, Diagnostics * diagnostics, CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens);

        SyntaxToken EatToken(SyntaxKind kind);
        SyntaxToken EatToken();
        SyntaxTokenCold GetColdToken(SyntaxToken token);
        SyntaxTokenCold PeekTokenCold(int32 steps);
        SyntaxToken PeekToken(int32 steps);

        SyntaxToken CreateMissingToken(SyntaxKind expected);
        SyntaxToken CreateMissingToken(SyntaxKind expected, SyntaxToken actual, bool reportError);
        Diagnostic MakeDiagnostic(ErrorCode code, SyntaxToken token);
        static ErrorCode GetExpectedTokenErrorCode(SyntaxKind expected, SyntaxKind actual);

        FixedCharSpan GetTokenText(SyntaxToken token);
        FixedCharSpan GetTokenTextWithTrivia(SyntaxToken token);

        SyntaxToken EatTokenWithPrejudice(SyntaxKind kind);

        void AddError(SyntaxToken token, Diagnostic diagnostic);

        void SkipToken();

        SyntaxToken GetLastNonSkipped();

        bool HasMoreTokens();
    };

}