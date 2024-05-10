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

    void AddTrivia(TriviaType type, FixedCharSpan span, bool isTrailing, PodList<Trivia>* triviaBuffer) {
        Trivia trivia;
        trivia.type = type;
        trivia.length = span.size;
        trivia.span = span.ptr;
        trivia.isTrailing = isTrailing;
        trivia.isLeading = !isTrailing;
        triviaBuffer->Add(trivia);
    }

    void LexMultiLineComment(TextWindow* textWindow, Diagnostics* diagnostics, bool isTrailing, PodList<Trivia>* triviaList) {
        bool isTerminated;
        FixedCharSpan span;
        ScanMultiLineComment(textWindow, &span, &isTerminated);
        if (!isTerminated) {
            // The comment didn't end.  Report an error at the start point.
            diagnostics->AddError(Diagnostic(ErrorCode::ERR_OpenEndedComment, span.ptr, textWindow->ptr));
        }

        AddTrivia(TriviaType::MultiLineComment, span, isTrailing, triviaList);
    }

    void LexSingleLineComment(TextWindow* textWindow, bool isTrailing, PodList<Trivia>* triviaList) {
        FixedCharSpan span;
        ScanSingleLineComment(textWindow, &span);
        AddTrivia(TriviaType::SingleLineComment, span, isTrailing, triviaList);
    }

    void LexDirectiveAndExcludedTrivia(bool afterFirstToken, bool afterNonWhitespaceOnLine, PodList<Trivia>* triviaList) {
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

    void LexConflictMarkerTrivia(TextWindow* textWindow, PodList<Trivia>* triviaList) {
        NOT_IMPLEMENTED("LexConflictMarkerTrivia");
    }

    void ScanEndOfLine(TextWindow * textWindow, FixedCharSpan * span) {
        char * start = textWindow->ptr;
        char c = textWindow->PeekChar();
        if(c == '\r') {
            textWindow->Advance();
            c = textWindow->PeekChar();
            if(c == '\n') {
                textWindow->Advance();
                *span = FixedCharSpan(start, (int32)(textWindow->ptr - start));
                return;
            }
        }
        char32 c32;
        int32 advance;
        textWindow->TryPeekChar32(&c32, &advance);
        if(IsNewline(c32)) {
            textWindow->Advance(advance);
            *span = FixedCharSpan(start, (int32)(textWindow->ptr - start));
        }
        else {
            *span = FixedCharSpan(start, 0);
        }
        return;
    }

    void LexSyntaxTrivia(TextWindow* textWindow, bool afterFirstToken, bool isTrailing, Diagnostics* diagnostics, PodList<Trivia>* triviaList) {
        bool onlyWhitespaceOnLine = !isTrailing;

        while (true) {

            char c = textWindow->PeekChar();
            if (c == ' ') {
                FixedCharSpan span;
                ScanWhitespace(textWindow, &span);
                AddTrivia(TriviaType::Whitespace, span, isTrailing, triviaList);
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
                    AddTrivia(TriviaType::Whitespace, span, isTrailing, triviaList);
                    break;
                }
                case '/': {
                    char c2 = textWindow->PeekAhead(1);
                    if (c2 == '/') {
                        // normal single line comment
                        LexSingleLineComment(textWindow, isTrailing, triviaList);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                    else if (c2 == '*') {
                        LexMultiLineComment(textWindow, diagnostics, isTrailing, triviaList);
                        onlyWhitespaceOnLine = false;
                        break;
                    }
                }
                case '\r':
                case '\n': {
                    FixedCharSpan line;
                    ScanEndOfLine(textWindow, &line);
                    AddTrivia(TriviaType::NewLine, line, isTrailing, triviaList);
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

    void PrintTokens(CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens, CheckedArray<LineColumn> lineCols) {

        for(int32 i = 0; i < hotTokens.size; i++) {
            //[line:column] kind, contextualKind -> "text"

            SyntaxToken token = hotTokens[i];
            SyntaxTokenCold cold = coldTokens[i];
            LineColumn lc = lineCols[i];
            printf("[%d:%d] %s (%s) -> \"%.*s\" \n", lc.line, lc.column, SyntaxKindToString(token.kind), SyntaxKindToString(token.contextualKind), cold.textSize, cold.text);

        }

    }

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> hotTokens, CheckedArray<SyntaxTokenCold> coldTokens, CheckedArray<LineColumn> lineCols) {

        // given a utf8 byte offset compute line/column for the token. tokens never span multiple lines (but trivia might)

        char* last = coldTokens[0].text;
        char* lastNewLineEnd = last;

        int32 lineNumber = 0;
        for (int32 i = 0; i < hotTokens.size; i++) {
            SyntaxTokenCold cold = coldTokens[i];

            char* ptr = last;
            while (ptr != cold.text) {

                if(*ptr == '\r') {
                    if(ptr + 1 != cold.text && ptr[1] == '\n') {
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
                // todo -- other line break chars, but they are 2 byte sequences i think
                if(*ptr == '\n') {
                    lineNumber++;
                    lastNewLineEnd = ptr + 1;
                }

                ptr++;
            }

            int32 col = (int32)(cold.text - lastNewLineEnd);

            // 1 based
            lineCols[i].line = lineNumber + 1;
            lineCols[i].column = col + 1;
            last = cold.text;


        }

    }

    void Tokenize(TextWindow * textWindow, LinearAllocator* allocator, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens, PodList<SyntaxTokenCold>* coldTokens, PodList<Trivia>* triviaBuffer) {

        int32 badTokenCount = 0;
        while (textWindow->HasMoreContent()) {

            int32 diagnosticCount = diagnostics->size;

            triviaBuffer->size = 0;
            LexSyntaxTrivia(textWindow, textWindow->ptr != textWindow->start, false, diagnostics, triviaBuffer);
            int32 leadingTrivia = triviaBuffer->size;

            TokenInfo tokenInfo;

            tokenInfo.text.ptr = textWindow->ptr;
            ScanSyntaxToken(textWindow, &tokenInfo, diagnostics, &badTokenCount);
            tokenInfo.text.size = (int32)(textWindow->ptr - tokenInfo.text.ptr);

            if (tokenInfo.valueKind != LiteralType::None) {
                Trivia literal;
                literal.type = TriviaType::LiteralValue;
                literal.literalValue = tokenInfo.literalValue;
                triviaBuffer->Add(literal);
            }

            LexSyntaxTrivia(textWindow, true, true, diagnostics, triviaBuffer);

            Trivia* trivia = allocator->AllocateUncleared<Trivia>(triviaBuffer->size);
            memcpy(trivia, triviaBuffer->array, triviaBuffer->size * sizeof(Trivia));

            SyntaxToken hotToken;
            hotToken.id = tokens->size;
            hotToken.kind = tokenInfo.kind;
            hotToken.contextualKind = tokenInfo.contextualKind;
            hotToken.literalType = tokenInfo.valueKind;
            hotToken.flags = SyntaxTokenFlags::None;

            if (leadingTrivia > 0) {
                hotToken.flags |= SyntaxTokenFlags::LeadingTrivia;
            }

            if (triviaBuffer->size != leadingTrivia) {
                hotToken.flags |= SyntaxTokenFlags::TrailingTrivia;
            }

            if (diagnostics->size != diagnosticCount) {
                hotToken.flags |= SyntaxTokenFlags::Error;
            }

            SyntaxTokenCold coldToken;
            coldToken.triviaList = trivia;
            coldToken.triviaCount = triviaBuffer->size;
            coldToken.triviaCapacity = triviaBuffer->size;
            coldToken.text = tokenInfo.text.ptr;
            coldToken.textSize = tokenInfo.text.size;

            tokens->Add(hotToken);
            coldTokens->Add(coldToken);

        }

    }

    void ScanSyntaxToken(TextWindow* textWindow, TokenInfo* info, Diagnostics* diagnostics, int32* badTokenCount) {
        info->kind = SyntaxKind::None;
        info->contextualKind = SyntaxKind::None;
        info->valueKind = LiteralType::None;
        info->literalValue.uint64Value = 0;
//        info->text.ptr = nullptr;
//        info->text.size = 0;

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
                    info->text = FixedCharSpan(textWindow->ptr, (int32) (textWindow->end - textWindow->ptr));
                    textWindow->ptr = textWindow->end;
                }
                else {
                    info->text = FixedCharSpan(start, (int32) (textWindow->ptr - start));
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
//        TokenInfo tokenInfo;
//
//        ScanSyntaxToken()
//
//    }


}
