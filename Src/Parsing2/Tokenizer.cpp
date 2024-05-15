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

    void AddTrivia(SyntaxKind triviaType, FixedCharSpan span, bool isTrailing, PodList<SyntaxToken>* buffer) {
        SyntaxToken token;
        token.text = span.ptr;
        token.textSize = span.size;
        token.kind = SyntaxKind::Trivia; // maybe we don't need this to be a kind
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

        AddTrivia(SyntaxKind::MultiLineComment, span, isTrailing, buffer);
    }

    void LexSingleLineComment(TextWindow* textWindow, bool isTrailing, PodList<SyntaxToken>* buffer) {
        FixedCharSpan span;
        ScanSingleLineComment(textWindow, &span);
        AddTrivia(SyntaxKind::SingleLineComment, span, isTrailing, buffer);
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
                AddTrivia(SyntaxKind::Whitespace, span, isTrailing, buffer);
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
                    AddTrivia(SyntaxKind::Whitespace, span, isTrailing, buffer);
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
                    AddTrivia(SyntaxKind::NewLine, line, isTrailing, buffer);
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

    void ScanStringLiteral(TextWindow* pWindow, SyntaxToken* pInfo, bool b) {

    }

    void PrintTokens(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols) {

        for (int32 i = 0; i < tokens.size; i++) {
            //[line:column] kind, contextualKind -> "text"

            SyntaxToken token = tokens[i];
            LineColumn lc = lineCols[i];
            printf("[%d:%d] %s (%s) -> \"%.*s\" \n", lc.line, lc.column, SyntaxKindToString(token.kind), SyntaxKindToString(token.contextualKind), token.textSize, token.text);

        }

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
            if(token.contextualKind == SyntaxKind::NewLine) {
                lc->endLine = line;
                lc->endColumn = column + token.textSize;
                column = 1;
                line++; // I think this is right
            }
            else if(token.contextualKind == SyntaxKind::MultiLineComment) {
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

    void ComputeTokenLineColumns2(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols) {

        char* last = tokens[0].text;
        char* lastNewLineEnd = last;

        int32 lineNumber = 0;
        for (int32 i = 0; i < tokens.size; i++) {
            SyntaxToken token = tokens[i];
            char* ptr = last;
            lineCols[i].line = lineNumber + 1;
            while (ptr != token.text) {

                if (*ptr == '\r') {
                    if (ptr + 1 != token.text && ptr[1] == '\n') {
                        lineNumber++;
                        lastNewLineEnd = ptr + 2; // we want byte width to end of the last new line char
                        ptr += 2;
                    }
                    else {
                        lineNumber++;
                        lastNewLineEnd = ptr;
                        ptr++;
                    }
                    continue;
                }
                if (*ptr == '\n') {
                    lineNumber++;
                    lastNewLineEnd = ptr + 1;
                }

                ptr++;
            }

            int32 col = (int32) (token.text - lastNewLineEnd);

            // 1 based
            lineCols[i].line = lineNumber + 1;
            lineCols[i].column = col + 1;
            last = token.text;

        }

    }

    void Tokenize(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {

        int32 badTokenCount = 0;
        while (textWindow->HasMoreContent()) {

            LexSyntaxTrivia(textWindow, textWindow->ptr != textWindow->start, false, diagnostics, tokens);

            SyntaxToken tokenInfo;

            tokenInfo.text = textWindow->ptr;
            ScanSyntaxToken(textWindow, &tokenInfo, diagnostics, &badTokenCount);
            tokenInfo.textSize = (int32) (textWindow->ptr - tokenInfo.text);

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
                ScanStringLiteral(textWindow, info, false);
                break;
            }
            case '/': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::SlashEqualsToken : SyntaxKind::SlashToken;
                break;
            }
            case '.': {
                if (!ScanNumericLiteral(textWindow, diagnostics, info)) {
                    textWindow->Advance();
                    info->kind = SyntaxKind::DotToken;
                    if (textWindow->TryAdvance('.')) {
                        info->kind = SyntaxKind::DotDotToken;
                        if (textWindow->PeekChar() == '.') {
                            info->kind = SyntaxKind::DotDotDotToken;
                        }
                    }
                }
                break;
            }
            case ',': {
                textWindow->Advance();
                info->kind = SyntaxKind::CommaToken;
                break;
            }
            case ':': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance(':') ? SyntaxKind::ColonColonToken : SyntaxKind::ColonToken;
                break;
            }
            case ';': {
                textWindow->Advance();
                info->kind = SyntaxKind::SemicolonToken;
                break;
            }
            case '~': {
                textWindow->Advance();
                info->kind = SyntaxKind::TildeToken;
                break;
            }
            case '!': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::ExclamationEqualsToken : SyntaxKind::ExclamationToken;
                break;
            }
            case '=': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::EqualsEqualsToken :
                    textWindow->TryAdvance('>') ? SyntaxKind::EqualsGreaterThanToken : SyntaxKind::EqualsToken;
                break;
            }
            case '*': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::AsteriskEqualsToken : SyntaxKind::AsteriskToken;
                break;
            }
            case '(': {
                textWindow->Advance();
                info->kind = SyntaxKind::OpenParenToken;
                break;
            }
            case ')': {
                textWindow->Advance();
                info->kind = SyntaxKind::CloseParenToken;
                break;
            }
            case '{': {
                textWindow->Advance();
                info->kind = SyntaxKind::OpenBraceToken;
                break;
            }

            case '}': {
                textWindow->Advance();
                info->kind = SyntaxKind::CloseBraceToken;
                break;
            }

            case '[': {
                textWindow->Advance();
                info->kind = SyntaxKind::OpenBracketToken;
                break;
            }

            case ']': {
                textWindow->Advance();
                info->kind = SyntaxKind::CloseBracketToken;
                break;
            }

            case '?': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('?')
                             ? textWindow->TryAdvance('=') ? SyntaxKind::QuestionQuestionEqualsToken : SyntaxKind::QuestionQuestionToken
                             : SyntaxKind::QuestionToken;
                break;
            }

            case '+': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::PlusEqualsToken :
                    textWindow->TryAdvance('+') ? SyntaxKind::PlusPlusToken : SyntaxKind::PlusToken;
                break;
            }

            case '-': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::MinusEqualsToken :
                    textWindow->TryAdvance('-') ? SyntaxKind::MinusMinusToken :
                    textWindow->TryAdvance('>') ? SyntaxKind::MinusGreaterThanToken : SyntaxKind::MinusToken;
                break;
            }

            case '%': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::PercentEqualsToken : SyntaxKind::PercentToken;
                break;
            }

            case '&': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::AmpersandEqualsToken :
                    textWindow->TryAdvance('&') ? SyntaxKind::AmpersandAmpersandToken : SyntaxKind::AmpersandToken;
                break;
            }

            case '^': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::CaretEqualsToken : SyntaxKind::CaretToken;
                break;
            }

            case '|': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::BarEqualsToken :
                    textWindow->TryAdvance('|') ? SyntaxKind::BarBarToken : SyntaxKind::BarToken;
                break;
            }

            case '<': {
                textWindow->Advance();
                info->kind =
                    textWindow->TryAdvance('=') ? SyntaxKind::LessThanEqualsToken :
                    textWindow->TryAdvance('<')
                    ? textWindow->TryAdvance('=') ? SyntaxKind::LessThanLessThanEqualsToken : SyntaxKind::LessThanLessThanToken
                    : SyntaxKind::LessThanToken;
                break;
            }

            case '>': {
                textWindow->Advance();
                info->kind = textWindow->TryAdvance('=') ? SyntaxKind::GreaterThanEqualsToken : SyntaxKind::GreaterThanToken;
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
