#include "../PrimitiveTypes.h"
#include "./SyntaxKind.h"
#include "./Scanning.h"
#include "../Collections/CheckedArray.h"
#include "./SyntaxFacts.h"
#include "../Collections/PodList.h"
#include "./Parser.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Compilation {

    // it would be nice if we could not have to deal w/ trivia while parsing
    // ideally we filter out non trivial tokens then when we're done parsing we write back the flags
    // we already have GetId() which gives us the token index
    bool TryFindNextNonTrivia(int32* ptr, CheckedArray<SyntaxToken> tokens) {
        int32 p = *ptr;
        for (int32 i = p + 1; i < tokens.size; i++) {
            if (tokens.array[i].kind == TokenKind::Trivia) {
                continue;
            }
            *ptr = i;
            return true;
        }
        return false;
    }

    SyntaxToken MakeEof() {
        SyntaxToken eof;
        eof.kind = TokenKind::EndOfFileToken;
        eof.SetFlags(SyntaxTokenFlags::Omitted);
        return eof;
    }

    Parser::Parser(LinearAllocator* allocator, TempAllocator* tempAllocator, Diagnostics* diagnostics, CheckedArray<SyntaxToken> tokens)
        : tokens(tokens)
        , ptr(0)
        , allocator(allocator)
        , tempAllocator(tempAllocator)
        , diagnostics(diagnostics)
        , termState(TerminatorState::EndOfFile)
        , forceConditionalAccessExpression(false)
        , currentToken() {

        for (ptr = 0; ptr < tokens.size; ptr++) {
            if (tokens.array[ptr].kind != TokenKind::Trivia) {
                break;
            }
        }

        if (ptr >= tokens.size) {
            ptr = tokens.size;
            currentToken = MakeEof();
        }
        else {
            currentToken = tokens[ptr];
        }

    }

    SyntaxToken Parser::PeekToken(int32 steps) {

        int32 p = ptr;

        for (int32 i = 0; i < steps; i++) {

            if (!TryFindNextNonTrivia(&p, tokens)) {
                return MakeEof();
            }

        }

        return tokens[p];

    }

    SyntaxToken Parser::EatToken(TokenKind kind) {
        if (currentToken.kind == kind) {
            return EatToken();
        }

        return CreateMissingToken(kind, currentToken, true);

    }

    SyntaxToken Parser::EatToken(TokenKind kind, ErrorCode errorCode, bool reportError) {
        if (currentToken.kind != kind) {
            return CreateMissingToken(kind, errorCode, reportError);
        }
        else {
            return EatToken();
        }

    }

    // similar to EatToken(kind) except doesn't create a missing, just adds an error and returns the actual token
    SyntaxToken Parser::EatTokenWithPrejudice(TokenKind kind) {
        SyntaxToken token = currentToken;

        if (token.kind != kind) {
            FixedCharSpan span = token.GetText();
            AddError(token, Diagnostic(GetExpectedTokenErrorCode(kind, token.kind), span.ptr, span.ptr + span.size));
        }

        EatToken();
        return token;
    }

    SyntaxToken Parser::SkipTokenWithPrejudice(TokenKind kind) {
        SyntaxToken token = currentToken;

        if (token.kind != kind) {
            FixedCharSpan span = token.GetText();
            AddError(token, Diagnostic(GetExpectedTokenErrorCode(kind, token.kind), span.ptr, span.ptr + span.size));
        }

        SkipToken();
        return token;
    }

    SyntaxToken Parser::EatToken() {
        SyntaxToken retn = currentToken;
        if (TryFindNextNonTrivia(&ptr, tokens)) {
            currentToken = tokens[ptr];
        }
        else {
            ptr = tokens.size;
            currentToken = MakeEof();
        }
        return retn;
    }

    // Consume a token if it is the right kind. Otherwise skip a token and replace it with one of the correct kind.
    SyntaxToken Parser::EatTokenAsKind(TokenKind expected) {
        assert(SyntaxFacts::IsToken(expected));

        SyntaxToken ct = currentToken;
        if (ct.kind == expected) {
            return EatToken();
        }

        SyntaxToken retn = CreateMissingToken(expected, GetExpectedTokenErrorCode(expected, currentToken.kind), true);
        SkipToken();
        return retn;
    }

    SyntaxToken Parser::CreateMissingToken(TokenKind expected) {
        SyntaxToken missing;
        missing.kind = expected;
        missing.SetFlags(SyntaxTokenFlags::Missing);
        missing.SetId(ptr);
        return missing;
    }

    SyntaxToken Parser::CreateMissingToken(TokenKind expected, ErrorCode errorCode, bool reportError) {
        SyntaxToken missing;
        missing.kind = expected;
        missing.SetFlags(SyntaxTokenFlags::Missing);
        missing.SetId(ptr);
        if (reportError) {
            AddError(missing, errorCode);
        }
        return missing;
    }

    SyntaxToken Parser::CreateMissingToken(TokenKind expected, SyntaxToken actual, bool reportError) {
        SyntaxToken missing = CreateMissingToken(expected);

        if (reportError) {
            AddError(actual, Diagnostic(GetExpectedTokenErrorCode(expected, actual.kind), actual.text, actual.text + actual.textSize));
        }

        return missing;
    }

    ErrorCode Parser::GetExpectedTokenErrorCode(TokenKind expected, TokenKind actual) {
        switch (expected) {
            case TokenKind::IdentifierToken:
                return SyntaxFacts::IsReservedKeyword(actual)
                    ? ErrorCode::ERR_IdentifierExpectedKW
                    : ErrorCode::ERR_IdentifierExpected;

            case TokenKind::SemicolonToken:
                return ErrorCode::ERR_SemicolonExpected;

            case TokenKind::CloseParenToken:
                return ErrorCode::ERR_CloseParenExpected;
            case TokenKind::OpenBraceToken:
                return ErrorCode::ERR_LbraceExpected;
            case TokenKind::CloseBraceToken:
                return ErrorCode::ERR_RbraceExpected;

            default:
                return ErrorCode::ERR_SyntaxError;
        }
    }

    Diagnostic Parser::MakeDiagnostic(ErrorCode code, SyntaxToken token) {
        return Diagnostic(code, token.text, token.text + token.textSize);
    }

    void Parser::AddError(SyntaxBase* node, ErrorCode errorCode) {
        if (node == nullptr) {
            return;
        }

        SyntaxToken start = tokens[node->startTokenId];
        SyntaxToken end = tokens[node->endTokenId];

        if (start.GetId() == ptr) {
            currentToken.AddFlag(SyntaxTokenFlags::Error);
        }

        if (start.GetId() >= 0) {
            tokens[start.GetId()].AddFlag(SyntaxTokenFlags::Error);
        }

        diagnostics->AddError(Diagnostic(errorCode, start.text, end.text + end.textSize));
    }

    void Parser::AddError(SyntaxToken token, ErrorCode errorCode) {
        if (token.GetId() == ptr) {
            currentToken.AddFlag(SyntaxTokenFlags::Error);
        }

        if (token.GetId() >= 0) {
            tokens[token.GetId()].AddFlag(SyntaxTokenFlags::Error);
        }

        diagnostics->AddError(Diagnostic(errorCode, token.text, token.text + token.textSize));
    }

    void Parser::AddError(SyntaxToken token, Diagnostic diagnostic) {

        if (token.GetId() == ptr) {
            currentToken.AddFlag(SyntaxTokenFlags::Error);
        }

        if (token.GetId() >= 0) {
            tokens[token.GetId()].AddFlag(SyntaxTokenFlags::Error);
        }

        diagnostics->AddError(diagnostic);
    }

    void Parser::SkipToken() {
        EatToken();
    }

    bool Parser::HasMoreTokens() {
        return ptr < tokens.size;
    }

    bool Parser::IsAfterNewLine(int32 idx) {
        for (int32 i = idx + 1; i < tokens.size; i++) {
            SyntaxToken t = tokens[i];
            if (t.kind != TokenKind::Trivia) {
                return false;
            }
            if (t.contextualKind == TokenKind::NewKeyword) {
                return true;
            }
        }
        return false;
    }

    bool Parser::NoTriviaBetween(SyntaxToken first, SyntaxToken second) {
        // simple check for now, could be crazier if needed
        return second.GetId() == first.GetId() + 1;
    }

    SyntaxToken Parser::EatContextualToken(TokenKind kind) {
        TokenKind contextualKind = currentToken.contextualKind;
        if (contextualKind != kind) {
            SyntaxToken retn = CreateMissingToken(kind);
            AddError(retn, GetExpectedTokenErrorCode(kind, contextualKind));
            return retn;
        }
        else {
            SyntaxToken keyword = EatToken();
            keyword.kind = kind;
            keyword.contextualKind = kind;
            return keyword;
        }
    }

    bool Parser::HasTrailingNewLine(SyntaxToken token) {
        for (int32 i = token.GetId(); i < tokens.size; i++) {
            SyntaxToken* s = &tokens[i];
            if ((s->GetFlags() & SyntaxTokenFlags::TrailingTrivia) == 0) {
                return false;
            }
            if (s->contextualKind == TokenKind::NewLine) {
                return true;
            }
        }

        return false;
    }

}

