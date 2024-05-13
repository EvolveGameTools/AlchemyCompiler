#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/CheckedArray.h"
#include "./Diagnostics.h"
#include "./Precidence.h"
#include "./TerminatorState.h"
#include "SyntaxBase.h"

namespace Alchemy::Compilation {

    SyntaxToken GetFirstToken(SyntaxBase * syntaxBase);
    SyntaxToken GetLastToken(SyntaxBase * syntaxBase);

    struct Parser  {

        SyntaxToken currentToken;
        int32 ptr;
        TerminatorState _termState;
        LinearAllocator* allocator;
        TempAllocator * tempAllocator;
        Diagnostics* diagnostics;
        CheckedArray<SyntaxToken> tokens;

        Parser(LinearAllocator * allocator, TempAllocator * tempAllocator, Diagnostics * diagnostics, CheckedArray<SyntaxToken> tokens);

        SyntaxToken EatToken(SyntaxKind kind);
        SyntaxToken EatToken();
        SyntaxToken PeekToken(int32 steps);

        SyntaxToken CreateMissingToken(SyntaxKind expected);
        SyntaxToken CreateMissingToken(SyntaxKind expected, SyntaxToken actual, bool reportError);
        Diagnostic MakeDiagnostic(ErrorCode code, SyntaxToken token);
        static ErrorCode GetExpectedTokenErrorCode(SyntaxKind expected, SyntaxKind actual);

        SyntaxToken EatTokenWithPrejudice(SyntaxKind kind);

        void AddError(SyntaxToken token, ErrorCode errorCode);
        void AddError(SyntaxToken token, Diagnostic diagnostic);
        void AddError(SyntaxBase* node, ErrorCode errorCode);

        void SkipToken();

        bool HasMoreTokens();

        template <typename T, typename... Args>
        T* CreateNode(Args && ... args) {
            T* retn = (T*) allocator->AllocateUncleared<T>(1);
            new(retn) T(std::forward<Args>(args)...);
            SyntaxBase * pBase = (SyntaxBase*)retn;
            SyntaxToken startToken = GetFirstToken(pBase);
            SyntaxToken endToken = GetLastToken(pBase);
            assert(startToken.IsValid() && !startToken.IsMissing());
            assert(endToken.IsValid());
            pBase->startTokenId = startToken.GetId();
            pBase->endTokenId = endToken.GetId();
            return retn;
        }


        bool IsAfterNewLine(int32 i);
    };

}