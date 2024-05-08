#include "../PrimitiveTypes.h"
#include "../Allocation/LinearAllocator.h"
#include "../Util/FixedCharSpan.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "../Unicode/Unicode.h"
#include "../Collections/PodList.h"
#include "./SyntaxKind.h"
#include "./SyntaxFacts.h"
#include "./TextWindow.h"
#include "./SyntaxDiagnosticInfo.h"
#include "./Scanning.h"
#include "Diagnostics.h"

namespace Alchemy::Compilation {

    struct SyntaxListBuilder {
    };

    struct Tokenizer {

        char* source;

        TextWindow textWindow;

        LinearAllocator* allocator;
        TempAllocator* tempAllocator;

        // I'd prefer these to come out of arenas but I'm not really sure how
        PodList<SyntaxDiagnosticInfo> errorList;
        PodList<Trivia> triviaList;

    };

    static Tokenizer* CreateTokenizer(TempAllocator* tempAllocator) {
        Tokenizer* tokenizer = tempAllocator->AllocateUncleared<Tokenizer>(1);
        return tokenizer;
    }

    int32 ht_lookup64(uint64 hash, int32 exp, int32 idx) {
        uint32 mask = ((uint32) 1 << exp) - 1;
        uint32 step = (hash >> (64 - exp)) | 1;
        return (int32) ((idx + step) & mask);
    }

    int32 ht_lookup32(int32 hash, int32 exp, int32 idx) {
        uint32 mask = ((uint32) 1 << exp) - 1;
        uint32 step = (hash >> (32 - exp)) | 1;
        return (int32) ((idx + step) & mask);
    }

    uint64 hash(char* s, int32 len) {
        uint64 h = 0x100;
        for (int32 i = 0; i < len; i++) {
            h ^= s[i] & 255;
            h *= 1111111111111111111;
        }
        return h ^ h >> 32;
    }

    bool IsContextualKeyword(SyntaxKind kind) {
        return kind > SyntaxKind::__FirstContextualKeyword__ && kind < SyntaxKind::__LastContextualKeyword__;
    }

    bool IsReservedKeyword(SyntaxKind kind) {
        return kind > SyntaxKind::__FirstKeyword__ && kind < SyntaxKind::__LastKeyword__;
    }

    void AddTrivia(TriviaType type, FixedCharSpan span, SyntaxListBuilder* listBuilder) {

    }

    void LexMultiLineComment(TextWindow* textWindow, Diagnostics * diagnostics,  SyntaxListBuilder* triviaList) {
        bool isTerminated;
        FixedCharSpan span;
        ScanMultiLineComment(textWindow, &span, &isTerminated);
        if (!isTerminated) {
            // The comment didn't end.  Report an error at the start point.
            diagnostics->AddScanningError(ScanningError(ErrorCode::ERR_OpenEndedComment, span.ptr, textWindow->ptr));
        }

        AddTrivia(TriviaType::MultiLineComment, span, triviaList);
    }

    void LexSingleLineComment(TextWindow* textWindow, SyntaxListBuilder* triviaList) {
        FixedCharSpan span;
        ScanSingleLineComment(textWindow, &span);
        AddTrivia(TriviaType::SingleLineComment, span, triviaList);
    }

    void LexDirectiveAndExcludedTrivia(bool afterFirstToken, bool afterNonWhitespaceOnLine, SyntaxListBuilder* triviaList) {
        NOT_IMPLEMENTED("LexDirectiveAndExcludedTrivia");
    }

    bool IsConflictMarkerTrivia(TextWindow* textWindow) {
        char* start = textWindow->ptr;

        int32 remaining = (int32) (textWindow->end - textWindow->ptr);

        if (remaining < 8) {
            return false;
        }

        if (textWindow->ptr == textWindow->start || IsNewline(textWindow->ptr[-1])) { // technically the -1 isn't getting us multichar, meh
            char firstChar = *textWindow->ptr;
            for (int32 i = 0; i < 7; i++) {
                if (textWindow->ptr[i] != firstChar) {
                    return false;
                }
            }
            if (firstChar == '|' || firstChar == '=') {
                return true;
            }

            return textWindow->ptr[7] == ' ';

        }

        return false;
    }

    void LexConflictMarkerTrivia(TextWindow* textWindow, SyntaxListBuilder* triviaList) {
        NOT_IMPLEMENTED("LexConflictMarkerTrivia");
    }

    void LexSyntaxTrivia(TextWindow* textWindow, bool afterFirstToken, bool isTrailing, Diagnostics * diagnostics, SyntaxListBuilder* triviaList) {
        bool onlyWhitespaceOnLine = !isTrailing;

        while (true) {

            char c = textWindow->PeekChar();
            if (c == ' ') {
                FixedCharSpan span;
                ScanWhitespace(textWindow, &span);
                AddTrivia(TriviaType::Whitespace, span, triviaList);
                continue;
            }
            else if (c > 127) {
                char32 c32;
                int32 advance;

                // we only need to check for multi-character whitespace & newlines and then we just set the trigger character for the switch that follows
                // Line separator character (U+2028) -> 8232
                // Paragraph separator character (U+2029) -> 8233
                if (textWindow->TryPeekChar32(&c32, &advance)) {
                    if (c32 == 8232 || c32 == 8233) {
                        c = '\n';
                    }
                    else if (Unicode::GetUnicodeCategory(c32) == Unicode::UnicodeCategory::SpaceSeparator) {
                        c = ' ';
                    }
                }

            }
            switch (c) {
                case ' ':
                case '\t':       // Horizontal tab
                case '\v':       // Vertical Tab
                case '\f': {     // Form-feed
                    FixedCharSpan span;
                    ScanWhitespace(textWindow, &span);
                    AddTrivia(TriviaType::Whitespace, span, triviaList);
                    break;
                }
                case '/': {
                    char c2 = textWindow->PeekAhead(1);
                    if (c2 == '/') {
                        // normal single line comment
                        LexSingleLineComment(textWindow, triviaList);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                    else if (c2 == '*') {
                        LexMultiLineComment(textWindow, diagnostics, triviaList);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                }
                case '\r':
                case '\n': {
                    FixedCharSpan line;
                    ScanToEndOfLine(textWindow, &line);
                    AddTrivia(TriviaType::NewLine, line, triviaList);
                    if (isTrailing) {
                        return;
                    }
                    onlyWhitespaceOnLine = true;
                    break;
                }
                case '#': {
                    LexDirectiveAndExcludedTrivia(afterFirstToken, isTrailing || !onlyWhitespaceOnLine, triviaList);
                    break;
                }
                case '|':
                case '=':
                case '<': {
                    if (!isTrailing) {
                        if (IsConflictMarkerTrivia(textWindow)) {
                            LexConflictMarkerTrivia(textWindow, triviaList);
                            break;
                        }
                    }
                    return;
                }
                default:
                    return;
            }
        }

    }

    void ScanStringLiteral(TextWindow* pWindow, TokenInfo* pInfo, bool b) {

    }

    bool ScanIdentifierOrKeyword(TextWindow* textWindow, TokenInfo* info) {

        FixedCharSpan identifier;

        if (ScanIdentifier_FastPath(textWindow, &identifier)) {
            info->text = identifier;
            // check if it's a keyword (no need to try if fast path didn't work)
            SyntaxKind keyword;
            if (TryMatchKeyword_Generated(identifier.ptr, identifier.size, &keyword)) {

                if (IsReservedKeyword(keyword)) {
                    info->Kind = keyword;
                    info->ContextualKind = SyntaxKind::None;
                }
                else {
                    // it's contextual
                    assert(IsContextualKeyword(keyword));
                    info->Kind = SyntaxKind::IdentifierToken;
                    info->ContextualKind = keyword;

                }

            }
            else {
                // not a keyword
                info->Kind = SyntaxKind::IdentifierToken;
                info->ContextualKind = SyntaxKind::IdentifierToken;
            }

            return true;

        }
        else if (ScanIdentifier_SlowPath(textWindow, &identifier)) {
            info->text = identifier;
            info->Kind = SyntaxKind::IdentifierToken;
            info->ContextualKind = SyntaxKind::IdentifierToken;
            return true;
        }
        else {
            info->Kind = SyntaxKind::None;
            info->ContextualKind = SyntaxKind::None;
            return false;
        }
    }

//    bool ScanNumericLiteral(TextWindow* textWindow, TokenInfo* info) {
//        return false;
//    }

    void Tokenize(char * src, int32 length, LinearAllocator * allocator) {
        TextWindow window(src, length);

    }

    void ScanSyntaxToken(TextWindow* textWindow, TokenInfo* info, Diagnostics * diagnostics, int32* badTokenCount) {
        info->Kind = SyntaxKind::None;
        info->ContextualKind = SyntaxKind::None;
        info->valueKind = LiteralType::None;
        info->literalValue.uint64Value = 0;
        info->text.ptr = nullptr;
        info->text.size = 0;

        char* start = textWindow->start;

        char character = textWindow->PeekChar();

        switch (character) {
            case '\"':
            case '\'': {
                ScanStringLiteral(textWindow, info, false);
                break;
            }
            case '/': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::SlashEqualsToken : SyntaxKind::SlashToken;
                break;
            }
            case '.': {
                if (!ScanNumericLiteral(textWindow, diagnostics, info)) {
                    textWindow->Advance();
                    info->Kind = SyntaxKind::DotToken;
                    if (textWindow->TryAdvance('.')) {
                        info->Kind = SyntaxKind::DotDotToken;
                        if (textWindow->PeekChar() == '.') {
                            info->Kind = SyntaxKind::DotDotDotToken;
                        }
                    }
                }
                break;
            }
            case ',': {
                textWindow->Advance();
                info->Kind = SyntaxKind::CommaToken;
                break;
            }
            case ':': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance(':') ? SyntaxKind::ColonColonToken : SyntaxKind::ColonToken;
                break;
            }
            case ';': {
                textWindow->Advance();
                info->Kind = SyntaxKind::SemicolonToken;
                break;
            }
            case '~': {
                textWindow->Advance();
                info->Kind = SyntaxKind::TildeToken;
                break;
            }
            case '!': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::ExclamationEqualsToken : SyntaxKind::ExclamationToken;
                break;
            }
            case '=': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::EqualsEqualsToken :
                        textWindow->TryAdvance('>') ? SyntaxKind::EqualsGreaterThanToken : SyntaxKind::EqualsToken;
                break;
            }
            case '*': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::AsteriskEqualsToken : SyntaxKind::AsteriskToken;
                break;
            }
            case '(': {
                textWindow->Advance();
                info->Kind = SyntaxKind::OpenParenToken;
                break;
            }
            case ')': {
                textWindow->Advance();
                info->Kind = SyntaxKind::CloseParenToken;
                break;
            }
            case '{': {
                textWindow->Advance();
                info->Kind = SyntaxKind::OpenBraceToken;
                break;
            }

            case '}': {
                textWindow->Advance();
                info->Kind = SyntaxKind::CloseBraceToken;
                break;
            }

            case '[': {
                textWindow->Advance();
                info->Kind = SyntaxKind::OpenBracketToken;
                break;
            }

            case ']': {
                textWindow->Advance();
                info->Kind = SyntaxKind::CloseBracketToken;
                break;
            }

            case '?': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('?')
                             ? textWindow->TryAdvance('=') ? SyntaxKind::QuestionQuestionEqualsToken : SyntaxKind::QuestionQuestionToken
                             : SyntaxKind::QuestionToken;
                break;
            }

            case '+': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::PlusEqualsToken :
                        textWindow->TryAdvance('+') ? SyntaxKind::PlusPlusToken : SyntaxKind::PlusToken;
                break;
            }

            case '-': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::MinusEqualsToken :
                        textWindow->TryAdvance('-') ? SyntaxKind::MinusMinusToken :
                        textWindow->TryAdvance('>') ? SyntaxKind::MinusGreaterThanToken : SyntaxKind::MinusToken;
                break;
            }

            case '%': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::PercentEqualsToken : SyntaxKind::PercentToken;
                break;
            }

            case '&': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::AmpersandEqualsToken :
                        textWindow->TryAdvance('&') ? SyntaxKind::AmpersandAmpersandToken : SyntaxKind::AmpersandToken;
                break;
            }

            case '^': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::CaretEqualsToken : SyntaxKind::CaretToken;
                break;
            }

            case '|': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::BarEqualsToken :
                        textWindow->TryAdvance('|') ? SyntaxKind::BarBarToken : SyntaxKind::BarToken;
                break;
            }

            case '<': {
                textWindow->Advance();
                info->Kind =
                        textWindow->TryAdvance('=') ? SyntaxKind::LessThanEqualsToken :
                        textWindow->TryAdvance('<')
                        ? textWindow->TryAdvance('=') ? SyntaxKind::LessThanLessThanEqualsToken : SyntaxKind::LessThanLessThanToken
                        : SyntaxKind::LessThanToken;
                break;
            }

            case '>': {
                textWindow->Advance();
                info->Kind = textWindow->TryAdvance('=') ? SyntaxKind::GreaterThanEqualsToken : SyntaxKind::GreaterThanToken;
                break;
            }

            case '@': {
                NOT_IMPLEMENTED("@ token handling");
                break;
            }

            case '$': {
                NOT_IMPLEMENTED("$ token handling");
                break;
            }

            default: {
                char32 c;
                int32 advance = 0;

                if (c >= '0' && c <= '9') {
                    ScanNumericLiteral(textWindow, diagnostics, info);
                    break;
                }

                textWindow->TryPeekChar32(&c, &advance);

                if (IsIdentifierStartCharacter(c)) {
                    ScanIdentifierOrKeyword(textWindow, info);
                    break;
                }

                textWindow->Advance(advance);

                *badTokenCount = *badTokenCount + 1;
                // If we get too many characters that we cannot make sense of, treat the entire rest of the file as
                // a single invalid character, so we can bail out of parsing early without producing an unbounded number of errors.
                if (*badTokenCount >= 200) {
                    info->text = FixedCharSpan(textWindow->ptr, (int32) (textWindow->end - textWindow->ptr));
                    textWindow->ptr = textWindow->end;
                }
                else {
                    info->text = FixedCharSpan(start, (int32) (textWindow->ptr - start));
                }

                diagnostics->AddScanningError(ScanningError(ErrorCode::ERR_UnexpectedCharacter, start, textWindow->ptr));

            }


        }


    }

//    void LexSyntaxToken(Tokenizer* tokenizer) {
//        tokenizer->leadingTriviaList.size = 0;
//
//        LexSyntaxTrivia(&tokenizer->textWindow, tokenizer->textWindow.ptr != tokenizer->textWindow.start, false, &tokenizer->leadingTriviaList);
//
//        TokenInfo tokenInfo;
//
//        ScanSyntaxToken()
//
//    }


}
