#include "../PrimitiveTypes.h"
#include "./SyntaxKind.h"
#include "./Scanning.h"
#include "../Collections/CheckedArray.h"
#include "./SyntaxFacts.h"
#include "../Collections/PodList.h"
#include "./Parser.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Compilation {

    Parser::Parser(LinearAllocator* allocator, TempAllocator* tempAllocator, Diagnostics* diagnostics, CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens)
        : hotTokens(hotTokens)
        , coldTokens(coldTokens)
        , ptr(0)
        , allocator(allocator)
        , tempAllocator(tempAllocator)
        , diagnostics(diagnostics)
        , _termState(TerminatorState::EndOfFile)
        , currentToken(hotTokens[0]) {}


    SyntaxToken MakeEof() {
        SyntaxToken eof;
        eof.id = -1; // its not really missing, its just kind of ephemeral
        eof.kind = SyntaxKind::EndOfFileToken;
        return eof;
    }

    SyntaxToken Parser::PeekToken(int32 steps) {
        if (ptr + steps >= hotTokens.size) {
            return MakeEof();
        }
        return hotTokens.array[ptr + steps];
    }

    SyntaxToken Parser::EatToken(SyntaxKind kind) {
        if (currentToken.kind == kind) {
            return EatToken();
        }

        return CreateMissingToken(kind, currentToken, true);

    }

    // similar to EatToken(kind) except doesn't create a missing, just adds an error and returns the actual token
    SyntaxToken Parser::EatTokenWithPrejudice(SyntaxKind kind) {
        SyntaxToken token = currentToken;

        if (token.kind != kind) {
            FixedCharSpan span = GetTokenText(token);
            AddError(token, Diagnostic(GetExpectedTokenErrorCode(kind, token.kind), span.ptr, span.ptr + span.size));
        }

        EatToken();
        return token;
    }

    SyntaxToken Parser::EatToken() {
        SyntaxToken retn = currentToken;

        if (ptr + 1 < hotTokens.size) {
            ptr++;
            currentToken = hotTokens.array[ptr];
        }
        else {
            currentToken = MakeEof();
            ptr = hotTokens.size;
        }

        return retn;

    }

    SyntaxTokenCold Parser::PeekTokenCold(int32 steps) {
        return coldTokens[PeekToken(steps).id];
    }

    SyntaxTokenCold Parser::GetColdToken(SyntaxToken token) {
        return token.id < 0
               ? SyntaxTokenCold()
               : coldTokens[token.id];
    }

    SyntaxToken Parser::CreateMissingToken(SyntaxKind expected) {
        SyntaxToken missing;
        missing.kind = expected;
        missing.flags |= SyntaxTokenFlags::Missing;
        missing.contextualKind = SyntaxKind::None;
        missing.id = -1;
        missing.literalType = LiteralType::None;
        return missing;
    }

    SyntaxToken Parser::CreateMissingToken(SyntaxKind expected, SyntaxToken actual, bool reportError) {
        SyntaxToken missing = CreateMissingToken(expected);

        if (reportError) {
            SyntaxTokenCold cold = GetColdToken(actual);
            AddError(actual, Diagnostic(GetExpectedTokenErrorCode(expected, actual.kind), cold.text, cold.text + cold.textSize));
        }

        return missing;
    }

    ErrorCode Parser::GetExpectedTokenErrorCode(SyntaxKind expected, SyntaxKind actual) {
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

    Diagnostic Parser::MakeDiagnostic(ErrorCode code, SyntaxToken token) {
        SyntaxTokenCold cold = GetColdToken(token);
        return Diagnostic(code, cold.text, cold.text + cold.textSize);
    }

    FixedCharSpan Parser::GetTokenTextWithTrivia(SyntaxToken token) {
        SyntaxTokenCold cold = GetColdToken(token);
        char* min = cold.text;
        char* max = cold.text + cold.textSize;

        for (int32 i = 0; i < cold.triviaCount; i++) {
            Trivia* trivia = &cold.triviaList[i];
            if (trivia->type != TriviaType::Whitespace) {
                continue;
            }

            if (trivia->span < min) {
                min = trivia->span;
            }
            if (trivia->span + trivia->length > max) {
                max = trivia->span + trivia->length;
            }
        }
        return FixedCharSpan(min, (int32) (max - min));
    }

    FixedCharSpan Parser::GetTokenText(SyntaxToken token) {
        SyntaxTokenCold cold = GetColdToken(token);
        return FixedCharSpan(cold.text, cold.textSize);
    }

    void Parser::AddError(SyntaxToken token, Diagnostic diagnostic) {

        if (token.id == ptr) {
            currentToken.flags |= SyntaxTokenFlags::Error;
        }

        if (token.id >= 0) {
            hotTokens.array[token.id].flags |= SyntaxTokenFlags::Error;
        }

        diagnostics->AddError(diagnostic);
    }

    SyntaxToken Parser::GetLastNonSkipped() {
        for (int32 i = ptr - 1; i >= 0; i--) {
            if ((hotTokens.array[i].flags & SyntaxTokenFlags::Skipped) == 0) {
                return hotTokens.array[i];
            }
        }

        UNREACHABLE("GetLastNonSkipped");

        return hotTokens[0];

    }

    void Parser::SkipToken() {
        EatToken();
        hotTokens[ptr - 1].flags |= SyntaxTokenFlags::Skipped;
    }

    bool Parser::HasMoreTokens() {
        return ptr < hotTokens.size;
    }


}

