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
        bool forceConditionalAccessExpression;

        Parser(LinearAllocator * allocator, TempAllocator * tempAllocator, Diagnostics * diagnostics, CheckedArray<SyntaxToken> tokens);

        SyntaxToken EatToken(TokenKind kind);
        SyntaxToken EatToken();
        SyntaxToken PeekToken(int32 steps);

        SyntaxToken CreateMissingToken(TokenKind expected);
        SyntaxToken CreateMissingToken(TokenKind expected, SyntaxToken actual, bool reportError);
        Diagnostic MakeDiagnostic(ErrorCode code, SyntaxToken token);
        static ErrorCode GetExpectedTokenErrorCode(TokenKind expected, TokenKind actual);

        SyntaxToken EatTokenWithPrejudice(TokenKind kind);

        void AddError(SyntaxToken token, ErrorCode errorCode);
        void AddError(SyntaxToken token, Diagnostic diagnostic);
        void AddError(SyntaxBase* node, ErrorCode errorCode);

        void SkipToken();
        SyntaxToken SkipTokenWithPrejudice(TokenKind kind);

        bool HasMoreTokens();

        template <typename T, typename... Args>
        T* CreateNode(Args && ... args) {
            T* retn = (T*) allocator->AllocateUncleared<T>(1);
            new(retn) T(std::forward<Args>(args)...);
            SyntaxBase * pBase = (SyntaxBase*)retn;
            SyntaxToken startToken = GetFirstToken(pBase);
            SyntaxToken endToken = GetLastToken(pBase);
            assert(startToken.IsValid());
            assert(endToken.IsValid());
            pBase->startTokenId = startToken.GetId();
            pBase->endTokenId = endToken.GetId();
            return retn;
        }


        bool IsAfterNewLine(int32 i);

        SyntaxToken EatToken(TokenKind kind, ErrorCode errorCode, bool reportError = true);

        SyntaxToken CreateMissingToken(TokenKind expected, ErrorCode errorCode, bool reportError);

        SyntaxToken EatTokenAsKind(TokenKind expected);

        bool NoTriviaBetween(SyntaxToken token, SyntaxToken token1);

        SyntaxToken EatContextualToken(TokenKind kind);


        bool HasTrailingNewLine(SyntaxToken token);
    };

}