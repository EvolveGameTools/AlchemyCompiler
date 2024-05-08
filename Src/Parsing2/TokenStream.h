#pragma once

#include "../Allocation/LinearAllocator.h"
#include "./SyntaxFacts.h"
#include "SyntaxDiagnosticInfo.h"

namespace Alchemy::Compilation {

    struct TokenStream {

        int32 ptr;
        SyntaxToken currentToken;
        CheckedArray<SyntaxToken> hotTokens;
        CheckedArray<SyntaxTokenCold> coldTokens;
        LinearAllocator* allocator;
        SyntaxDiagnosticInfo * prevDiagnostic;

        TokenStream(LinearAllocator* allocator, CheckedArray<SyntaxToken> tokens, CheckedArray<SyntaxTokenCold> coldTokens)
                : allocator(allocator)
                  , hotTokens(tokens)
                  , coldTokens(coldTokens)
                  , ptr(0)
                  , prevDiagnostic(nullptr) {
            currentToken = tokens[0];
        }

        SyntaxTokenCold GetColdToken(SyntaxToken token) {
            if (token.id < 0) {
                return *(SyntaxTokenCold*) (allocator->GetBase() + (-token.id));
            }
            return coldTokens[token.id];
        }

        SyntaxTokenCold PeekTokenCold(int32 steps) {
            return coldTokens[PeekToken(steps).id];
        }

        SyntaxToken PeekToken(int32 steps) {
            if (ptr + steps >= hotTokens.size) {
                return hotTokens.array[hotTokens.size - 1]; // eof
            }
            return hotTokens.array[ptr + steps];
        }


        SyntaxDiagnosticInfo* MakeExpectedTokenError(SyntaxKind expected, SyntaxKind actual, int32 offset, int32 width) {
            ErrorCode code = GetExpectedTokenErrorCode(expected, actual);
            SyntaxDiagnosticInfo * error = allocator->Allocate<SyntaxDiagnosticInfo>(1);
            error->offset = offset;
            error->errorCode = code;
            error->prevDiagnostic = prevDiagnostic;
            error->diagnosticType = DiagnosticType::Error;

            prevDiagnostic = error;

            if (code == ErrorCode::ERR_SyntaxError) {
                error->text = (char*)SyntaxFacts::GetText(expected);
                error->textLength = (int32)strlen(error->text);
            }
            else if (code == ErrorCode::ERR_IdentifierExpectedKW) {
                error->text = (char*)SyntaxFacts::GetText(actual);
                error->textLength = (int32)strlen(error->text);
            }
            else {
                error->text = nullptr;
                error->textLength = 0;
            }

            return error;
        }

        ErrorCode GetExpectedTokenErrorCode(SyntaxKind expected, SyntaxKind actual) {
            switch (expected) {
                case SyntaxKind::IdentifierToken:
                    return SyntaxFacts::IsReservedKeyword(actual)
                           ? ErrorCode::ERR_IdentifierExpectedKW
                           : ErrorCode::ERR_IdentifierExpected;

                case SyntaxKind::SemicolonToken:
                    return ErrorCode::ERR_SemicolonExpected;

                case SyntaxKind::CloseParenToken:
                    return ErrorCode::ERR_CloseParenExpected;
                case SyntaxKind::OpenBraceToken:
                    return ErrorCode::ERR_LbraceExpected;
                case SyntaxKind::CloseBraceToken:
                    return ErrorCode::ERR_RbraceExpected;

                default:
                    return ErrorCode::ERR_SyntaxError;
            }
        }

        SyntaxToken CreateMissingToken(SyntaxKind expected, SyntaxKind actual, bool reportError) {

            SyntaxTokenCold* cold = allocator->Allocate<SyntaxTokenCold>(1);
            cold->text = (char*) "";
            cold->triviaList = nullptr;
            cold->triviaCapacity = 0;
            cold->triviaCount = 0;
            cold->textSize = 0;
            SyntaxToken missing;
            missing.kind = expected;
            missing.flags |= SyntaxTokenFlags::Missing;
            missing.contextualKind = SyntaxKind::None;
            missing.id = -(int32) allocator->GetOffset(cold);
            missing.literalType = LiteralType::None;

            if (reportError) {
                missing.flags |= SyntaxTokenFlags::Error;
                Trivia* trivia = allocator->Allocate<Trivia>(1);
                trivia->diagnostic = MakeExpectedTokenError(expected, actual, 0, 0);
                trivia->type = TriviaType::Diagnostic;
                trivia->isLeading = false;
                trivia->isTrailing = false;
                trivia->length = 0;
            }

            return missing;
        }

        SyntaxToken EatToken(SyntaxKind kind) {
            if (currentToken.kind == kind) {
                return EatToken();
            }

            return CreateMissingToken(kind, currentToken.kind, true);

        }

        SyntaxToken EatToken() {
            if (ptr + 1 >= hotTokens.size) {
                return hotTokens.array[hotTokens.size - 1]; // eof
            }
            currentToken = hotTokens.array[ptr++];
            return currentToken;
        }

    };

}