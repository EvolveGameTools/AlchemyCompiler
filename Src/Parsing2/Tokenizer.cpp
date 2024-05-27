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

    bool IsAtEnd(bool allowNewLines) {
        return false;
    }

    void ScanInterpolatedStringLiteralHoleBalancedText(TextWindow* textWindow, char endingChar, bool isHole) {
        while (true) {

            if (IsAtEnd(true)) {
                return; // caller will complain & set error
            }

            char ch = textWindow->PeekChar();

            switch (ch) {
                case '#': {
                    // preprocessor is not allowed
                    // TrySetError();
                    textWindow->Advance();
                    continue;
                }
                case '$': {

                    break;
                }
                case ':': {
                    if (isHole) {
                        SyntaxToken colon;
                        colon.text = textWindow->ptr;
                        colon.textSize = 1;
                        colon.kind = TokenKind::ColonToken;
                        // ScanFormatSpecifier();
                    }
                    textWindow->Advance();
                    continue;
                }
                case '}':
                case ')':
                case ']': {
                    if (ch == endingChar) {
                        return;
                    }
                    Diagnostic diagnostic;
                    diagnostic.errorCode = ErrorCode::ERR_SyntaxError;
                    diagnostic.start = textWindow->ptr;
                    diagnostic.end = textWindow->ptr + 1;
                    // TrySetError(_lexer.MakeError(_lexer.TextWindow.Position, 1, ErrorCode::ERR_SyntaxError, endingChar.ToString()));
                    textWindow->Advance();
                    continue;
                }
                case '"': {
                    if (hasError) {
                        return;
                    }
                    // handle string literal inside an expression hole.
                    ScanInterpolatedStringLiteralNestedString();
                    continue;
                }
                case '/': {
                    switch (textWindow->PeekAhead(1)) {
                        case '/': {
                            FixedCharSpan span;
                            ScanToEndOfLine(textWindow, &span);
                            continue;
                        }
                        case '*': {
                            bool isTerminated = true;
                            FixedCharSpan span;
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
                    ScanInterpolatedStringLiteralHoleBracketed('{', '}');
                    continue;
                }
                case '(': {
                    ScanInterpolatedStringLiteralHoleBracketed('(', ')');
                    continue;
                }
                case '[': {
                    ScanInterpolatedStringLiteralHoleBracketed('[', ']');
                    continue;
                }
                default: {
                    textWindow->Advance();
                    continue;
                }
            }

        }
    }

    void ScanCharacterLiteral(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* token) {
        char* start = textWindow->start;

        assert(*textWindow->ptr == '\'');
        // scan until new line or '
        textWindow->Advance();

        bool isTerminated = false;

        while (textWindow->HasMoreContent()) {

            if (textWindow->PeekChar() == '\'') {
                isTerminated = true;
                break;
            }

            if(textWindow->PeekChar() == '\r' || textWindow->PeekChar() == '\n' || textWindow->PeekChar() == '\0') {
                break;
            }

        }

        token->kind = TokenKind::CharacterLiteralToken;
        token->textSize = (int32) (textWindow->ptr - start);
        token->text = start;
        if(!isTerminated) {
            Diagnostic diagnostic;
            diagnostic.start = start;
            diagnostic.end = textWindow->ptr;
            diagnostic.errorCode = ErrorCode::ERR_UnterminatedCharacterLiteral;
            diagnostics->AddError(diagnostic);
        }

    }

    void ScanStringInterpolationHole(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens, bool allowMultiLine, TextWindow* retn) {
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
                    SyntaxToken token;
                    ScanCharacterLiteral(textWindow, diagnostics, &token);
                    continue;
                }
                case '/': {
                    switch (textWindow->PeekAhead(1)) {
                        case '/': {
                            // end of line
                            FixedCharSpan span;
                            ScanToEndOfLine(textWindow, &span);
                            // todo -- probably want to mark this as unterminated if single line
                            continue;
                        }
                        case '*': {
                            FixedCharSpan span;
                            bool isTerminated = false;
                            ScanMultiLineComment(textWindow, &span, &isTerminated);
                            // todo -- probably want to check for newlines if not allowed
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

    void LexStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, PodList<SyntaxToken>* tokens) {
        assert(textWindow->PeekChar() == '"');
        bool secondIsQuote = textWindow->PeekAhead(1) == '"';
        bool thirdIsQuote = textWindow->PeekAhead(2) == '"';
        char* start = textWindow->ptr;
        if (secondIsQuote && thirdIsQuote) {
            // multi string
        }
        else if (secondIsQuote) {
            // empty string
            SyntaxToken token;
            token.kind = TokenKind::StringLiteralEmpty;
            token.text = start;
            token.textSize = 2;
            tokens->Add(token);
            textWindow->Advance(2);
            return;
        }
        else {
            // single string
            SyntaxToken startToken;
            startToken.kind = TokenKind::StringLiteralStart;
            startToken.text = start;
            startToken.textSize = 1;
            tokens->Add(startToken);

            SyntaxToken content;
            // we won't scan escapes other than \$
            textWindow->Advance();
            char last = '\0';
            char c = textWindow->PeekChar();


            // if we're at the end of file or we hit a new line that isn't allowed, we're done
            // if(IsAtEnd()) {
            //
            // }

            while (c != '\0') {
                switch (c) {
                    case '\n':
                    case '\r': {
                        Diagnostic diagnostic;
                        diagnostic.start = start;
                        diagnostic.end = textWindow->ptr;
                        diagnostic.errorCode = ErrorCode::ERR_NewlineInConst;
                        diagnostics->AddError(diagnostic);
                        goto end;
                    }
                    case '$': {
                        if (last == '\\') {
                            break; // escaped, just keep going
                        }

                        if (textWindow->PeekAhead(1) == '{') {
                            // template hole
                            // Where the contents contains zero or more sequences
                            //                ${ STUFF }
                            // where these curly braces delimit STUFF in expression "holes".
                            // In order to properly find the closing quote of the whole string,
                            // we need to locate the closing brace of each hole, as strings
                            // may appear in expressions in the holes. So we
                            // need to match up any braces that appear between them.
                            // But in order to do that, we also need to match up any
                            // /**/ comments, ' characters quotes, () parens
                            // [] brackets, and "" strings, including interpolated holes in the latter.

                            SyntaxToken interpolationStart;
                            interpolationStart.kind = TokenKind::InterpolatedExpressionStart;
                            interpolationStart.text = textWindow->ptr;
                            interpolationStart.textSize = 2;

                            // we want to sub-tokenize these
                            // we don't accept // comments
                            // we don't accept new lines right?

                            textWindow->Advance(); // step over {

                            TextWindow holeWindow(nullptr, 0);
                            ScanStringInterpolationHole(textWindow, diagnostics, tokens, &holeWindow);
                            Tokenize(&holeWindow, diagnostics, tokens);

                            // textWindow points at the } now (or '\0' if we ran out of things to tokenize)
                            SyntaxToken interpolationEnd;
                            interpolationStart.kind = TokenKind::InterpolatedExpressionEnd;
                            interpolationStart.text = textWindow->ptr;
                            interpolationStart.textSize = 1;


                            break;
                        }


                        SyntaxToken dollar;
                        dollar.kind = TokenKind::InterpolatedIdentifier;
                        dollar.textSize = 1;
                        dollar.text = textWindow->ptr;

                        SyntaxToken token;
                        if (ScanIdentifierOrKeyword(textWindow, &token)) {
                            tokens->Add(dollar);
                            tokens->Add(token);
                        }

                        // it was nothing, just move on
                        break;

                    }
                    default: {
                        break;
                    }
                }
                last = c;
                c = textWindow->PeekChar();
            }

            if (!textWindow->HasMoreContent()) {
                // failed to find end of string
            }
        }
        end:

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

            }

            else if (*textWindow->ptr == '\'') {

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

    void ScanVerbatimStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info) {
        char* start = textWindow->ptr;
        while (textWindow->PeekChar() == '@') {
            textWindow->Advance();
        }

        if (textWindow->ptr - start >= 2) {
            Diagnostic diagnostic;
            diagnostic.start = start;
            diagnostic.errorCode = ErrorCode::ERR_IllegalAtSequence;
            diagnostic.end = textWindow->ptr;
            diagnostics->AddError(diagnostic);
        }

        assert(textWindow->PeekChar() == '"');
        textWindow->Advance();

        while (true) {
            char ch = textWindow->PeekChar();

            if (ch == '"') {

                textWindow->Advance();
                if (textWindow->PeekChar() == '"') {
                    // Doubled quote -- skip & keep going. -- we'll need to re-process this when emitting the string contents I think
                    textWindow->Advance();
                    continue;
                }

                // otherwise, the string is finished.
                break;
            }

            if (ch == '\0') {
                // Reached the end of the source without finding the end-quote.  Give an error back at the
                // starting point. And finish lexing this string.
                Diagnostic diagnostic;
                diagnostic.start = start;
                diagnostic.errorCode = ErrorCode::ERR_UnterminatedStringLiteral;
                diagnostic.end = textWindow->ptr;
                diagnostics->AddError(diagnostic);
                break;
            }

            textWindow->Advance();
        }

        info->kind = TokenKind::VerbatimStringLiteral;
        info->textSize = (int32) (textWindow->ptr - start);
        info->text = start;

    }

    enum class InterpolatedStringKind {
        // Normal interpolated string that just starts with <c>$"</c>
        Normal,
        // Verbatim interpolated string that starts with <c>$@"</c> or <c>@$"</c>
        Verbatim,
        // Single-line raw interpolated string that starts with at least one <c>$</c>, and at least three <c>"</c>s.
        SingleLineRaw,
        // Multi-line raw interpolated string that starts with at least one <c>$</c>, and at least three <c>"</c>s.
        MultiLineRaw,
    };

    struct InterpolatedStringScanner {

        Diagnostics* diagnostics;
        TextWindow* textWindow;
        Diagnostic diagnostic;
        bool hasError;

        void TrySetError(Diagnostic error) {
            // only need to record the first error we hit
            if (hasError) {
                return;
            }
            hasError = true;
            diagnostic = error;
        }

        bool IsNewLine() {
            char ch = textWindow->PeekChar();
            return ch == '\r' || ch == '\n';
        }

        bool IsAtEnd(InterpolatedStringKind kind) {
            return IsAtEnd(kind == InterpolatedStringKind::Verbatim || kind == InterpolatedStringKind::MultiLineRaw);
        }

        bool IsAtEnd(bool allowNewline) {
            char ch = textWindow->PeekChar();
            if (ch == '\0') {
                return true;
            }
            return (!allowNewline && IsNewLine());
        }

        void ScanInterpolatedStringLiteralTop(
            InterpolatedStringKind* kind,
            SyntaxToken openQuoteRange,
            SyntaxToken closeQuoteRange
        ) {

            char* start = textWindow->ptr;
        }

        bool ScanOpenQuote(InterpolatedStringKind* kind) {
            // verbatim start
            // interpolated start -> we remove and just scan ${} and $identifier
            // raw strings still use ${} if you don't want interpolated
        }

    };

    void ScanInterpolatedStringLiteralTop(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info
//        ref TokenInfo info,
//        out SyntaxDiagnosticInfo? error,
//    out InterpolatedStringKind kind,
//    out Range openQuoteRange,
//    ArrayBuilder<Interpolation>? interpolations,
//    out Range closeQuoteRange
    ) {
//    var subScanner = new InterpolatedStringScanner(this);
//    subScanner.ScanInterpolatedStringLiteralTop(out kind, out openQuoteRange, interpolations, out closeQuoteRange);
//    error = subScanner.Error;
//    info->kind = TokenKind::InterpolatedStringToken;
//    info.Text = TextWindow.GetText(intern: false);
    }

    void ScanInterpolatedStringLiteral(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info) {
        // We have a string of one of the forms
        //                $" ... "
        //                $@" ... "
        //                @$" ... "
        // Where the contents contains zero or more sequences
        //                { STUFF }
        // where these curly braces delimit STUFF in expression "holes".
        // In order to properly find the closing quote of the whole string,
        // we need to locate the closing brace of each hole, as strings
        // may appear in expressions in the holes. So we
        // need to match up any braces that appear between them.
        // But in order to do that, we also need to match up any
        // /**/ comments, ' characters quotes, () parens
        // [] brackets, and "" strings, including interpolated holes in the latter.

//        ScanInterpolatedStringLiteralTop(
//            ref
//        info,
//            out
//        var error,
//        kind:
//        out _,
//        openQuoteRange:
//        out _,
//        interpolations:
//        null,
//            closeQuoteRange: out
//        _);
//        this.AddError(error);
    }

    bool TryScanAtStringToken(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info) {
        int32 index = 0;
        while (textWindow->PeekAhead(index) == '@') {
            index++;
        }

        if (textWindow->PeekAhead(index) == '"') {
            // @"
            ScanVerbatimStringLiteral(textWindow, diagnostics, info);
            return true;
        }
        else if (textWindow->PeekAhead(index) == '$') {
            // @$"
            ScanInterpolatedStringLiteral(textWindow, info);
            return true;
        }

        return false;
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
