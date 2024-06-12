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

    void AddTrivia(TokenKind triviaType, FixedCharSpan span, bool isTrailing, PodList<SyntaxToken>* tokens) {
        SyntaxToken token;
        token.text = span.ptr;
        token.textSize = span.size;
        token.kind = TokenKind::Trivia; // maybe we don't need this to be a kind
        token.contextualKind = triviaType;
        token.SetFlags(isTrailing ? SyntaxTokenFlags::TrailingTrivia : SyntaxTokenFlags::LeadingTrivia);
        tokens->Add(token);
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

    void ComputeTokenLineColumns(CheckedArray<SyntaxToken> tokens, CheckedArray<LineColumn> lineCols) {
        int32 column = 1;
        int32 line = 1;

        for (int32 i = 0; i < tokens.size; i++) {
            // multi line comment is the only weird one
            SyntaxToken token = tokens[i];
            LineColumn* lc = &lineCols[i];
            lc->line = line;
            lc->column = column;
            if (token.contextualKind == TokenKind::NewLine) {
                lc->endLine = line;
                lc->endColumn = column + token.textSize;
                column = 1;
                line++; // I think this is right
            }
            else if (token.contextualKind == TokenKind::MultiLineComment) {
                for (int32 s = 0; s < token.textSize; s++) {
                    if (token.text[s] == '\n') {
                        line++;
                    }
                }
                lc->endLine = line;
                lc->endColumn = column + token.textSize;
                if (line != lc->line) {
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

    void ScanStringInterpolationHole(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens, TextWindow* retn) {
        int32 brace = 1;

        int32 tokenCount = tokens->size;
        int32 diagnosticCount = diagnostics->size;

        char* start = textWindow->ptr;

        while (true) {
            char c = textWindow->PeekChar();
            switch (c) {
                case '"': {
                    LexStringLiteral(textWindow, diagnostics, tokens);
                    continue;
                }
                case '\'': {
                    LexCharacterLiteral(textWindow, diagnostics, tokens);
                    continue;
                }
                case '/': {
                    switch (textWindow->PeekAhead(1)) {
                        case '/': {
                            // end of line
                            FixedCharSpan span;
                            ScanToEndOfLine(textWindow, &span);
                            continue;
                        }
                        case '*': {
                            FixedCharSpan span;
                            bool isTerminated = false;
                            ScanMultiLineComment(textWindow, &span, &isTerminated);
                            continue;
                        }
                        default: {
                            textWindow->Advance();
                            continue;
                        }
                    }
                }
                case '{': {
                    brace++;
                    textWindow->Advance();
                    continue;
                }
                case '}': {
                    brace--;
                    if (brace == 0) {
                        goto end;
                    }
                    textWindow->Advance();
                    continue;
                }
                case '\0': {
                    // unterminated, return I guess.
                    goto end;
                }
                default: {
                    textWindow->Advance();
                    continue;
                }

                    // ignoring (), [], and <> because it just gives bad errors, better to do a greedy match

            }
        }

        end:
        diagnostics->size = diagnosticCount;
        tokens->size = tokenCount;

        // we are pointing at } or \0

        retn->ptr = start;
        retn->start = start;
        retn->end = textWindow->ptr;

    }

    int32 CountQuoteSequence(TextWindow* textWindow) {
        char* ptr = textWindow->ptr;

        while (ptr != textWindow->end && *ptr == '"') {
            ptr++;
        }
        return (int32) (ptr - textWindow->ptr);
    }

    void LexRawStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {
        char* start = textWindow->ptr;

        int32 cnt = CountQuoteSequence(textWindow);
        SyntaxToken startToken;
        startToken.kind = TokenKind::RawStringLiteralStart;
        startToken.text = start;
        startToken.textSize = cnt;
        tokens->Add(startToken);

        textWindow->ptr += cnt;

        char last = '\0';
        char c = textWindow->PeekChar();
        SyntaxToken stringPart;
        stringPart.kind = TokenKind::StringLiteralPart;
        stringPart.textSize = 0;
        stringPart.text = textWindow->ptr;

        while (true) {
            bool addToStringPart = false;

            switch (c) {
                case '$': {
                    if (last == '\\') {
                        addToStringPart = true;
                        break; // escaped \$, just keep going
                    }

                    // ${ marks the start of an interpolation
                    if (textWindow->PeekAhead(1) == '{') {

                        if (stringPart.textSize != 0) {
                            tokens->Add(stringPart);
                        }

                        SyntaxToken interpolationStart;
                        interpolationStart.kind = TokenKind::InterpolatedExpressionStart;
                        interpolationStart.contextualKind = TokenKind::InterpolatedExpressionStart;
                        interpolationStart.text = textWindow->ptr;
                        interpolationStart.textSize = 2; // ${

                        tokens->Add(interpolationStart);

                        // we want to sub-tokenize these

                        textWindow->Advance(2); // step over ${

                        TextWindow holeWindow(nullptr, 0);
                        // scan ahead and find the matching } greedily
                        ScanStringInterpolationHole(textWindow, diagnostics, tokens, &holeWindow);
                        Tokenize(&holeWindow, diagnostics, tokens);

                        // textWindow points at the } now (or '\0' if we ran out of things to tokenize)
                        SyntaxToken interpolationEnd;
                        interpolationStart.kind = TokenKind::InterpolatedExpressionEnd;
                        interpolationStart.contextualKind = TokenKind::InterpolatedExpressionEnd;
                        interpolationStart.text = textWindow->ptr;
                        interpolationStart.textSize = 1; // }
                        tokens->Add(interpolationEnd);

                        stringPart.text = textWindow->ptr + 1; // we'll advance after break but set this here
                        stringPart.textSize = 0;

                        break;
                    }

                    char* tokenText = textWindow->ptr;
                    textWindow->Advance(); // step over $
                    SyntaxToken token;
                    if (ScanIdentifierOrKeyword(textWindow, &token)) {
                        token.kind = TokenKind::InterpolatedIdentifier;
                        token.contextualKind = TokenKind::InterpolatedIdentifier;
                        token.text = tokenText;
                        token.textSize += 1;
                        // add accumulated string parts
                        if (stringPart.textSize > 0) {
                            tokens->Add(stringPart);
                        }
                        stringPart.text = textWindow->ptr;
                        stringPart.textSize = 0;
                        textWindow->ptr--; // don't over advance

                        tokens->Add(token);
                    }
                    else {
                        // it was nothing, just move on, reset to $ for normal handling
                        textWindow->ptr = tokenText;
                        addToStringPart = true;
                    }
                    break;
                }
                case '"': {
                    int32 sequence = CountQuoteSequence(textWindow);
                    if (sequence == cnt) {
                        // done
                        if (stringPart.textSize > 0) {
                            tokens->Add(stringPart);
                        }
                        SyntaxToken endToken;
                        endToken.kind = TokenKind::RawStringLiteralEnd;
                        endToken.contextualKind = TokenKind::RawStringLiteralEnd;
                        endToken.text = textWindow->ptr;
                        endToken.textSize = 1; // "
                        tokens->Add(endToken);
                        textWindow->Advance();
                        return;
                    }
                    else {
                        // keep going, we only add 1 instead of the whole sequence so that we can deal with
                        // strings being too long to fit inside uint16 storage.
                        addToStringPart = true;
                        break;
                    }

                }
                case '\0': {
                    // done, unterminated
                    if (stringPart.textSize > 0) {
                        tokens->Add(stringPart);
                    }
                    SyntaxToken endToken;
                    endToken.kind = TokenKind::RawStringLiteralEnd;
                    endToken.contextualKind = TokenKind::RawStringLiteralEnd;
                    endToken.text = textWindow->ptr;
                    endToken.textSize = 0;
                    endToken.SetFlags(SyntaxTokenFlags::Missing);
                    tokens->Add(endToken);
                    Diagnostic diagnostic(ErrorCode::ERR_UnterminatedStringLiteral, start, textWindow->ptr);
                    diagnostics->AddError(diagnostic);
                    return;
                }
                default: {
                    addToStringPart = true;
                    break;
                }
            }
            last = c;
            textWindow->Advance();
            c = textWindow->PeekChar();
            if(addToStringPart) {
                if (stringPart.textSize + 1 == UINT16_MAX) {
                    // if we have a string that is too long, make another
                    tokens->Add(stringPart);
                    stringPart.textSize = 0;
                    stringPart.text = textWindow->ptr;
                }
                stringPart.textSize++;
            }
        }

    }

    void LexStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {
        assert(textWindow->PeekChar() == '"');
        bool secondIsQuote = textWindow->PeekAhead(1) == '"';
        bool thirdIsQuote = textWindow->PeekAhead(2) == '"';
        char* start = textWindow->ptr;

        if (secondIsQuote && thirdIsQuote) {
            // raw multi string
            // delimited by quote count
            LexRawStringLiteral(textWindow, diagnostics, tokens);
        }
        else if (secondIsQuote) {
            // empty string
            SyntaxToken token;
            token.kind = TokenKind::StringLiteralEmpty;
            token.contextualKind = TokenKind::StringLiteralEmpty;
            token.text = start;
            token.textSize = 2; // ""
            tokens->Add(token);
            textWindow->Advance(2);
            return;
        }
        else {
            // single string
            SyntaxToken startToken;
            startToken.kind = TokenKind::StringLiteralStart;
            startToken.contextualKind = TokenKind::StringLiteralStart;
            startToken.text = start;
            startToken.textSize = 1; // "
            tokens->Add(startToken);

            // we won't scan escapes other than \$, parser / code generator will need to do that
            textWindow->Advance();
            char last = '\0';
            char c = textWindow->PeekChar();
            SyntaxToken stringPart;
            stringPart.kind = TokenKind::StringLiteralPart;
            stringPart.contextualKind = TokenKind::StringLiteralPart;
            stringPart.textSize = 0;
            stringPart.text = textWindow->ptr;

            while (true) {
                bool addToStringPart = false;

                switch (c) {
                    case '\n':
                    case '\r': {
                        // todo -- we don't need to do this, Rust doesn't. Multiline strings are fine in rust
                        if (stringPart.textSize > 0) {
                            tokens->Add(stringPart);
                        }
                        SyntaxToken endToken;
                        endToken.kind = TokenKind::StringLiteralEnd;
                        endToken.contextualKind = TokenKind::StringLiteralEnd;
                        endToken.text = textWindow->ptr;
                        endToken.textSize = 0; // missing "
                        endToken.SetFlags(SyntaxTokenFlags::Missing);
                        tokens->Add(endToken);

                        Diagnostic diagnostic;
                        diagnostic.start = start;
                        diagnostic.end = textWindow->ptr;
                        diagnostic.errorCode = ErrorCode::ERR_NewlineInConst;
                        diagnostics->AddError(diagnostic);
                        if (c == '\r' && textWindow->PeekAhead(1) == '\n') {
                            textWindow->Advance(2);
                        }
                        else {
                            textWindow->Advance();
                        }
                        return;
                    }
                    case '$': {

                        if (last == '\\') {
                            addToStringPart = true;
                            break; // escaped \$, just keep going
                        }

                        // ${ marks the start of an interpolation
                        if (textWindow->PeekAhead(1) == '{') {

                            if (stringPart.textSize != 0) {
                                tokens->Add(stringPart);
                            }

                            SyntaxToken interpolationStart;
                            interpolationStart.kind = TokenKind::InterpolatedExpressionStart;
                            interpolationStart.contextualKind = TokenKind::InterpolatedExpressionStart;
                            interpolationStart.text = textWindow->ptr;
                            interpolationStart.textSize = 2; // ${

                            tokens->Add(interpolationStart);

                            // we want to sub-tokenize these

                            textWindow->Advance(2); // step over ${

                            TextWindow holeWindow(nullptr, 0);
                            // scan ahead and find the matching } greedily
                            ScanStringInterpolationHole(textWindow, diagnostics, tokens, &holeWindow);
                            Tokenize(&holeWindow, diagnostics, tokens);

                            // textWindow points at the } now (or '\0' if we ran out of things to tokenize)
                            SyntaxToken interpolationEnd;
                            interpolationEnd.kind = TokenKind::InterpolatedExpressionEnd;
                            interpolationEnd.contextualKind = TokenKind::InterpolatedExpressionEnd;
                            interpolationEnd.text = textWindow->ptr;
                            interpolationEnd.textSize = 1; // }
                            tokens->Add(interpolationEnd);

                            stringPart.text = textWindow->ptr + 1; // we'll advance after break but set this here
                            stringPart.textSize = 0;

                            break;
                        }

                        char* tokenText = textWindow->ptr;
                        textWindow->Advance(); // step over $
                        SyntaxToken token;
                        if (ScanIdentifierOrKeyword(textWindow, &token)) {
                            token.kind = TokenKind::InterpolatedIdentifier;
                            token.text = tokenText;
                            token.textSize += 1;
                            // add accumulated string parts
                            if (stringPart.textSize > 0) {
                                tokens->Add(stringPart);
                            }

                            stringPart.text = textWindow->ptr;
                            stringPart.textSize = 0;

                            textWindow->ptr--; // we'll over advance below if we don't step back here
                            tokens->Add(token);
                        }
                        else {
                            // it was nothing, just move on, reset to $ for normal handling
                            textWindow->ptr = tokenText;
                            addToStringPart = true;
                        }
                        break;

                    }
                    case '\0': {
                        // done, unterminated
                        if (stringPart.textSize > 0) {
                            tokens->Add(stringPart);
                        }
                        SyntaxToken endToken;
                        endToken.kind = TokenKind::StringLiteralEnd;
                        endToken.contextualKind = TokenKind::StringLiteralEnd;
                        endToken.text = textWindow->ptr;
                        endToken.textSize = 0;
                        endToken.SetFlags(SyntaxTokenFlags::Missing);
                        tokens->Add(endToken);
                        Diagnostic diagnostic(ErrorCode::ERR_UnterminatedStringLiteral, start, textWindow->ptr);
                        diagnostics->AddError(diagnostic);
                        return;
                    }
                    case '"': {
                        if (last != '\\') {
                            // done
                            if (stringPart.textSize > 0) {
                                tokens->Add(stringPart);
                            }
                            SyntaxToken endToken;
                            endToken.kind = TokenKind::StringLiteralEnd;
                            endToken.contextualKind = TokenKind::StringLiteralEnd;
                            endToken.text = textWindow->ptr;
                            endToken.textSize = 1; // "
                            tokens->Add(endToken);
                            textWindow->Advance();
                            return;
                        }
                        else {
                            addToStringPart = true;
                            break;
                        }
                    }
                    default: {
                        addToStringPart = true;
                        break;
                    }
                }
                last = c;
                textWindow->Advance();
                c = textWindow->PeekChar();
                if(addToStringPart) {
                    if (stringPart.textSize + 1 == UINT16_MAX) {
                        // if we have a string that is too long, make another
                        tokens->Add(stringPart);
                        stringPart.textSize = 0;
                        stringPart.text = textWindow->ptr;
                    }
                    stringPart.textSize++;
                }
            }

        }

    }

    void Tokenize(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {

        int32 badTokenCount = 0;
        while (textWindow->HasMoreContent()) {

            LexSyntaxTrivia(textWindow, textWindow->ptr != textWindow->start, false, diagnostics, tokens);

            SyntaxToken tokenInfo;

            tokenInfo.text = textWindow->ptr;
            if (*textWindow->ptr == '\"') {
                // we have 2 string types: "normal single line strings" and """ unescaped strings """
                // $identifier is an interpolation template
                // ${expression} is also an interpolation template
                // \$ escapes in both cases

                // we either have 1 quote or 3, anything else is an error
                LexStringLiteral(textWindow, diagnostics, tokens);
                LexSyntaxTrivia(textWindow, true, true, diagnostics, tokens);
                continue;
            }
            else if(*textWindow->ptr == '\'') {
                LexCharacterLiteral(textWindow, diagnostics, tokens);
                LexSyntaxTrivia(textWindow, true, true, diagnostics, tokens);
                continue;
            }
            else {
                ScanSyntaxToken(textWindow, &tokenInfo, diagnostics, &badTokenCount);
            }
            tokenInfo.textSize = (int32) (textWindow->ptr - tokenInfo.text);

            if (tokenInfo.contextualKind == TokenKind::None) {
                tokenInfo.contextualKind = tokenInfo.kind;
            }

            tokens->Add(tokenInfo);

            LexSyntaxTrivia(textWindow, true, true, diagnostics, tokens);

        }

        for (int32 i = 0; i < tokens->size; i++) {
            tokens->Get(i).SetId(i);
        }

    }

    void LexCharacterLiteral(TextWindow* textWindow, Diagnostics * diagnostics, PodList<SyntaxToken> * tokens) {
        // read until unescaped ' or end of line

        SyntaxToken start;
        start.kind = TokenKind::CharLiteralStart;
        start.contextualKind = TokenKind::CharLiteralStart;
        start.text = textWindow->ptr;
        start.textSize = 1;
        tokens->Add(start);
        textWindow->Advance();

        char * charStart = textWindow->ptr;

        SyntaxToken content;
        content.kind = TokenKind::CharLiteralContent;
        content.contextualKind = TokenKind::CharLiteralContent;
        content.text = charStart;

        char last = '\0';
        while(true) {
            char c = textWindow->PeekChar();
            if(c == '\'' && last != '\\') {
                // if we had '' don't add it, let the parser mark that as an error
                int32 size = (int32)(textWindow->ptr - charStart);
                if(size > 0) {
                    content.textSize = size;
                    tokens->Add(content);
                }
                SyntaxToken end;
                end.kind = TokenKind::CharLiteralEnd;
                end.contextualKind = TokenKind::CharLiteralEnd;
                end.text = textWindow->ptr;
                end.textSize = 1;
                tokens->Add(end);
                textWindow->Advance(); // step over '
                return;
            }
            else if (c == '\0' || c == '\r' || c == '\n') {
                int32 size = (int32)(textWindow->ptr - charStart);
                if(size > 0) {
                    content.textSize = size;
                    tokens->Add(content);
                }
                SyntaxToken end;
                end.kind = TokenKind::CharLiteralEnd;
                end.contextualKind = TokenKind::CharLiteralEnd;
                end.text = textWindow->ptr;
                end.textSize = 0;
                diagnostics->AddError(Diagnostic(ErrorCode::ERR_UnterminatedCharacterLiteral, start.text, textWindow->ptr));
                tokens->Add(end);
                // don't advance
                return;
            }
            else {
                last = c;
                textWindow->Advance();
            }
        }

    }

    void ScanSyntaxToken(TextWindow* textWindow, SyntaxToken* info, Diagnostics* diagnostics, int32* badTokenCount) {

        char* start = textWindow->start;

        char character = textWindow->PeekChar();

        switch (character) {
            case '\"':
                UNREACHABLE("Cannot scan string literals");
                break;
            case '\'': {

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
