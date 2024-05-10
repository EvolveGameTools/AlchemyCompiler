#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/CheckedArray.h"
#include "./Diagnostics.h"
#include "./Precidence.h"
#include "./TerminatorState.h"
#include "SyntaxBase.h"

namespace Alchemy::Compilation {

    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase);

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

        template <typename T, typename... Args>
        T* CreateNode(Args && ... args) {
            T* retn = (T*) allocator->AllocateUncleared<T>(1);
            new(retn) T(std::forward<Args>(args)...);
            SyntaxBase * pBase = (SyntaxBase*)retn;
            SyntaxToken startToken = GetFirstToken(pBase);
            pBase->startTokenId = startToken.id;

            assert(startToken.IsValid() && !startToken.IsMissing());

            pBase->endTokenId = ptr;
            return retn;
        }
    };

}