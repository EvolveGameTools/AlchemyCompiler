#include "../PrimitiveTypes.h"
#include "../Allocation/LinearAllocator.h"
#include "../Util/FixedCharSpan.h"
#include "../Unicode/Unicode.h"
#include "../Collections/PodList.h"
#include "./SyntaxKind.h"
#include "./TextWindow.h"
#include "./Scanning.h"
#include "./Tokenizer.h"
#include <cstdio>

namespace Alchemy::Compilation {

    void AddTrivia(TokenKind triviaType, FixedCharSpan span, bool isTrailing, PodList<SyntaxToken>* buffer) {
        SyntaxToken token;
        token.text = span.ptr;
        token.textSize = span.size;
        token.kind = TokenKind::Trivia; // maybe we don't need this to be a kind
        token.contextualKind = triviaType;
        token.SetFlags(isTrailing ? SyntaxTokenFlags::TrailingTrivia : SyntaxTokenFlags::LeadingTrivia);
        buffer->Add(token);
    }

    void LexMultiLineComment(TextWindow* textWindow, Diagnostics* diagnostics, bool isTrailing, PodList<SyntaxToken>* buffer) {
        bool isTerminated;
        FixedCharSpan span;
        ScanMultiLineComment(textWindow, &span, &isTerminated);
        if (!isTerminated) {
            // The comment didn't end.  Report an error at the start point.
            diagnostics->AddError(Diagnostic(ErrorCode::ERR_OpenEndedComment, span.ptr, textWindow->ptr));
        }

        AddTrivia(TokenKind::MultiLineComment, span, isTrailing, buffer);
    }

    void LexSingleLineComment(TextWindow* textWindow, bool isTrailing, PodList<SyntaxToken>* buffer) {
        FixedCharSpan span;
        ScanSingleLineComment(textWindow, &span);
        AddTrivia(TokenKind::SingleLineComment, span, isTrailing, buffer);
    }

    void LexDirectiveAndExcludedTrivia(bool afterFirstToken, bool afterNonWhitespaceOnLine, PodList<SyntaxToken>* buffer) {
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

    void LexConflictMarkerTrivia(TextWindow* textWindow, PodList<SyntaxToken>* buffer) {
        NOT_IMPLEMENTED("LexConflictMarkerTrivia");
    }

    void ScanEndOfLine(TextWindow* textWindow, FixedCharSpan* span) {
        char* start = textWindow->ptr;
        char c = textWindow->PeekChar();
        if (c == '\r') {
            textWindow->Advance();
            c = textWindow->PeekChar();
            if (c == '\n') {
                textWindow->Advance();
                *span = FixedCharSpan(start, (int32) (textWindow->ptr - start));
                return;
            }
        }
        char32 c32;
        int32 advance;
        textWindow->TryPeekChar32(&c32, &advance);
        if (IsNewline(c32)) {
            textWindow->Advance(advance);
            *span = FixedCharSpan(start, (int32) (textWindow->ptr - start));
        }
        else {
            *span = FixedCharSpan(start, 0);
        }
        return;
    }

    void LexSyntaxTrivia(TextWindow* textWindow, bool afterFirstToken, bool isTrailing, Diagnostics* diagnostics, PodList<SyntaxToken>* buffer) {
        bool onlyWhitespaceOnLine = !isTrailing;

        while (true) {

            char c = textWindow->PeekChar();
            if (c == ' ') {
                FixedCharSpan span;
                ScanWhitespace(textWindow, &span);
                AddTrivia(TokenKind::Whitespace, span, isTrailing, buffer);
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
                    AddTrivia(TokenKind::Whitespace, span, isTrailing, buffer);
                    break;
                }
                case '/': {
                    char c2 = textWindow->PeekAhead(1);
                    if (c2 == '/') {
                        // normal single line comment
                        LexSingleLineComment(textWindow, isTrailing, buffer);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                    else if (c2 == '*') {
                        LexMultiLineComment(textWindow, diagnostics, isTrailing, buffer);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                }
                case '\r':
                case '\n': {
                    FixedCharSpan line;
                    ScanEndOfLine(textWindow, &line);
                    AddTrivia(TokenKind::NewLine, line, isTrailing, buffer);
                    if (isTrailing) {
                        return;
                    }
                    onlyWhitespaceOnLine = true;
                    break;
                }
                case '#': {
                    LexDirectiveAndExcludedTrivia(afterFirstToken, isTrailing || !onlyWhitespaceOnLine, buffer);
                    break;
                }
                case '|':
                case '=':
                case '<': {
                    if (!isTrailing) {
                        if (IsConflictMarkerTrivia(textWindow)) {
                            LexConflictMarkerTrivia(textWindow, buffer);
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

    void ScanStringLiteral(TextWindow* textWindow, SyntaxToken* pInfo, bool inDirective) {

    }

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols) {
        int32 column = 1;
        int32 line = 1;

        for (int32 i = 0; i < tokens.size; i++) {
            // multi line comment is the only weird one
            SyntaxToken token = tokens[i];
            LineColumn * lc = &lineCols[i];
            lc->line = line;
            lc->column = column;
            if(token.contextualKind == TokenKind::NewLine) {
                lc->endLine = line;
                lc->endColumn = column + token.textSize;
                column = 1;
                line++; // I think this is right
            }
            else if(token.contextualKind == TokenKind::MultiLineComment) {
                for(int32 s = 0; s < token.textSize; s++) {
                    if(token.text[s] == '\n') {
                        line++;
                    }
                }
                lc->endLine = line;
                lc->endColumn = column + token.textSize;
                if(line != lc->line) {
                    column = 1;
                }
            }
            else {
                column += token.textSize;
                // token won't break across lines
                lc->endLine = line;
                lc->endColumn = column;
            }
        }

    }

    void Tokenize(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {

        int32 badTokenCount = 0;
        while (textWindow->HasMoreContent()) {

            LexSyntaxTrivia(textWindow, textWindow->ptr != textWindow->start, false, diagnostics, tokens);

            SyntaxToken tokenInfo;

            tokenInfo.text = textWindow->ptr;
            if(*textWindow->ptr == '\"') {
                // %#"multiline string no need to escape"
                // #rm"" raw multiline
                // rm"" raw multiline
                // r"" raw string
                // r#"" raw string
                // m""
                // rm$ "" raw multiline with interpolations
                // r"
                // somehow we need to search the holes in our string for ${ } pairings
                // "{value} $value {{value}} "
                // " string "
                // r" raw string "
                // """ multiline string """
                // """ raw multiline string """
                // r#$""" raw multi line string {{with interpolations} }"""
                // @ multi line string
                // @""
            }
            else if(*textWindow->ptr == '\'') {

            }
            else if(*textWindow->ptr == '`') {

            }
            else {
                ScanSyntaxToken(textWindow, &tokenInfo, diagnostics, &badTokenCount);
            }
            tokenInfo.textSize = (int32) (textWindow->ptr - tokenInfo.text);

            if(tokenInfo.contextualKind == TokenKind::None) {
                tokenInfo.contextualKind = tokenInfo.kind;
            }

            tokens->Add(tokenInfo);

            LexSyntaxTrivia(textWindow, true, true, diagnostics, tokens);

        }

        for (int32 i = 0; i < tokens->size; i++) {
            tokens->Get(i).SetId(i);
        }

    }

    void ScanSyntaxToken(TextWindow* textWindow, SyntaxToken* info, Diagnostics* diagnostics, int32* badTokenCount) {

        char* start = textWindow->start;

        char character = textWindow->PeekChar();

        switch (character) {
            case '\"':
            case '\'': {
                UNREACHABLE("Cannot scan string or character literals");
                break;
            }
            case '/': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::SlashEqualsToken : TokenKind::SlashToken;
                break;
            }
            case '.': {
                if (!ScanNumericLiteral(textWindow, diagnostics, info)) {
                    textWindow->Advance();
                    info->kind = TokenKind::DotToken;
                    if (textWindow->TryAdvance('.')) {
                        info->kind = TokenKind::DotDotToken;
                        if (textWindow->PeekChar() == '.') {
                            info->kind = TokenKind::DotDotDotToken;
                        }
                    }
                }
                break;
            }
            case ',': {
                textWindow->Advance();
                info->kind = TokenKind::CommaToken;
                break;
            }
            case ':': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance(':') ? TokenKind::ColonColonToken : TokenKind::ColonToken;
                break;
            }
            case ';': {
                textWindow->Advance();
                info->kind = TokenKind::SemicolonToken;
                break;
            }
            case '~': {
                textWindow->Advance();
                info->kind = TokenKind::TildeToken;
                break;
            }
            case '!': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::ExclamationEqualsToken : TokenKind::ExclamationToken;
                break;
            }
            case '=': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::EqualsEqualsToken :
                    textWindow->TryAdvance('>') ? TokenKind::EqualsGreaterThanToken : TokenKind::EqualsToken;
                break;
            }
            case '*': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::AsteriskEqualsToken : TokenKind::AsteriskToken;
                break;
            }
            case '(': {
                textWindow->Advance();
                info->kind = TokenKind::OpenParenToken;
                break;
            }
            case ')': {
                textWindow->Advance();
                info->kind = TokenKind::CloseParenToken;
                break;
            }
            case '{': {
                textWindow->Advance();
                info->kind = TokenKind::OpenBraceToken;
                break;
            }

            case '}': {
                textWindow->Advance();
                info->kind = TokenKind::CloseBraceToken;
                break;
            }

            case '[': {
                textWindow->Advance();
                info->kind = TokenKind::OpenBracketToken;
                break;
            }

            case ']': {
                textWindow->Advance();
                info->kind = TokenKind::CloseBracketToken;
                break;
            }

            case '?': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('?')
                             ? textWindow->TryAdvance('=') ? TokenKind::QuestionQuestionEqualsToken : TokenKind::QuestionQuestionToken
                             : TokenKind::QuestionToken;
                break;
            }

            case '+': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::PlusEqualsToken :
                    textWindow->TryAdvance('+') ? TokenKind::PlusPlusToken : TokenKind::PlusToken;
                break;
            }

            case '-': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::MinusEqualsToken :
                    textWindow->TryAdvance('-') ? TokenKind::MinusMinusToken :
                    textWindow->TryAdvance('>') ? TokenKind::MinusGreaterThanToken : TokenKind::MinusToken;
                break;
            }

            case '%': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::PercentEqualsToken : TokenKind::PercentToken;
                break;
            }

            case '&': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::AmpersandEqualsToken :
                    textWindow->TryAdvance('&') ? TokenKind::AmpersandAmpersandToken : TokenKind::AmpersandToken;
                break;
            }

            case '^': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::CaretEqualsToken : TokenKind::CaretToken;
                break;
            }

            case '|': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::BarEqualsToken :
                    textWindow->TryAdvance('|') ? TokenKind::BarBarToken : TokenKind::BarToken;
                break;
            }

            case '<': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? TokenKind::LessThanEqualsToken :
                    textWindow->TryAdvance('<')
                    ? textWindow->TryAdvance('=') ? TokenKind::LessThanLessThanEqualsToken : TokenKind::LessThanLessThanToken
                    : TokenKind::LessThanToken;
                break;
            }

            case '>': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? TokenKind::GreaterThanEqualsToken : TokenKind::GreaterThanToken;
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

                textWindow->TryPeekChar32(&c, &advance);

                if (c >= '0' && c <= '9') {
                    ScanNumericLiteral(textWindow, diagnostics, info);
                    break;
                }

                if (IsIdentifierStartCharacter(c)) {
                    ScanIdentifierOrKeyword(textWindow, info);
                    break;
                }

                textWindow->Advance(advance);

                *badTokenCount = *badTokenCount + 1;
                // If we get too many characters that we cannot make sense of, treat the entire rest of the file as
                // a single invalid character, so we can bail out of parsing early without producing an unbounded number of errors.
                if (*badTokenCount >= 200) {
                    info->text = textWindow->ptr;
                    info->textSize = (int32) (textWindow->end - textWindow->ptr);
                    textWindow->ptr = textWindow->end;
                }
                else {
                    info->text = start;
                    info->textSize = (int32) (textWindow->ptr - start);
                }

                diagnostics->AddError(Diagnostic(ErrorCode::ERR_UnexpectedCharacter, start, textWindow->ptr));

            }


        }


    }

//    void LexSyntaxToken(Tokenizer* tokenizer) {
//        tokenizer->leadingTriviaList.size = 0;
//
//        LexSyntaxTrivia(&tokenizer->textWindow, tokenizer->textWindow.ptr != tokenizer->textWindow.start, false, &tokenizer->leadingTriviaList);
//
//        SyntaxToken tokenInfo;
//
//        ScanSyntaxToken()
//
//    }


}
