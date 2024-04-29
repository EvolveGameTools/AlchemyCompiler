#include "TextWindow.h"
#include "SyntaxDiagnosticInfo.h"
#include "SyntaxFacts.h"
#include "../Util/FixedCharSpan.h"
#include "../Unicode/Unicode.h"
#include "Scanning.h"
#include "../Collections/FixedPodList.h"
#include "../Allocation/ThreadLocalTemp.h"

namespace Alchemy::Parsing {

    SyntaxDiagnosticInfo CreateIllegalEscapeDiagnostic(TextWindow* window, char* start) {
        return {window->start, start, window->ptr, ErrorCode::IllegalEscape};
    }

    uint32 ScanUnicodeEscape(TextWindow* textWindow, SyntaxDiagnosticInfo* error) {
        char* start = textWindow->ptr;
        if (*start != '\\') {
            return 0;
        }
        textWindow->Advance();
        char c = textWindow->PeekChar();
        uint32 retn = 0;

        if (c == 'U') {
            textWindow->Advance();

            if (!SyntaxFacts::IsHexDigit(textWindow->PeekChar())) {
                *error = CreateIllegalEscapeDiagnostic(textWindow, start);
                return 0;
            }
            else {
                for (int32 i = 0; i < 8; i++) {
                    char character = textWindow->PeekChar();
                    if (!SyntaxFacts::IsHexDigit(character)) {
                        *error = CreateIllegalEscapeDiagnostic(textWindow, start);
                        return 0;
                    }

                    retn = (uint32) ((retn << 4) + SyntaxFacts::HexValue(character));
                    textWindow->Advance();
                }

            }

            if (retn > 0x0010FFFF) {
                *error = CreateIllegalEscapeDiagnostic(textWindow, start);
                return 0;
            }

            return retn;

        }
        else if (c == 'u') {

            textWindow->Advance();
            if (!SyntaxFacts::IsHexDigit(textWindow->PeekChar())) {
                *error = CreateIllegalEscapeDiagnostic(textWindow, start);
                return 0;
            }
            else {
                for (int32 i = 0; i < 4; i++) {

                    char ch2 = textWindow->PeekChar();

                    if (!SyntaxFacts::IsHexDigit(ch2)) {
                        *error = CreateIllegalEscapeDiagnostic(textWindow, start);
                        return 0;
                    }

                    retn = (retn << 4) + SyntaxFacts::HexValue(ch2);
                    textWindow->Advance();
                }

            }

        }

        return retn;
    }

    bool IsIdentifierStartCharacter(uint32 codepoint) {
        return (codepoint >= 'A' && codepoint <= 'Z') || (codepoint >= 'a' && codepoint <= 'z') || codepoint == '_' || Alchemy::Unicode::IsLetter(codepoint);
    }

    void ScanNumericLiteralSingleInteger(TextWindow* textWindow, Alchemy::FixedPodList<char>* buffer, bool* underscoreInWrongPlace, bool* usedUnderscore, bool* firstCharWasUnderscore, bool isHex, bool isBinary) {
        if (textWindow->Peek('_')) {
            if (isHex || isBinary) {
                *firstCharWasUnderscore = true;
            }
            else {
                *underscoreInWrongPlace = true;
            }
        }
        bool lastCharWasUnderscore = false;
        while (true) {
            char ch = textWindow->PeekChar();
            if (ch == '_') {
                *usedUnderscore = true;
                lastCharWasUnderscore = true;
            }
            else if (!(isHex ? SyntaxFacts::IsHexDigit(ch) : isBinary
                                                             ? SyntaxFacts::IsBinaryDigit(ch)
                                                             : SyntaxFacts::IsDecDigit(ch))) {
                break;
            }
            else {
                buffer->Add(ch);
                lastCharWasUnderscore = false;
            }
            textWindow->Advance();
        }

        if (lastCharWasUnderscore) {
            *underscoreInWrongPlace = true;
        }
    }

    static int32 BufferNumber(char* str, int32 length, char* buffer, int32 bufferLimit) {
        int32 write = 0;
        int32 max = length < bufferLimit - 1 ? length : bufferLimit - 1;
        for (int32 i = 0; i < max; i++) {
            if (str[i] != '_') {
                buffer[write++] = str[i];
            }
        }
        buffer[write] = '\0';
        return write;
    }


    uint64 GetValueUInt64(FixedPodList<char>* pList, bool isHex, bool isBinary) {
        uint64 result;
        if (isBinary) {
            if (!TryParseBinaryUInt64(pList, &result)) {
                AddError(MakeError(ErrorCode::ERR_IntOverflow));
            }
        }
        else if(isHex) {

        }
        else {

        }
        else if (!UInt64.TryParse(text, isHex ? NumberStyles.AllowHexSpecifier : NumberStyles.None, CultureInfo.InvariantCulture, &result)) {
            //we've already lexed the literal, so the error must be from overflow
            AddError(MakeError(ErrorCode::ERR_IntOverflow));
        }

        return result;
    }

    double GetDoubleValue(FixedPodList<char>* pList) {
        char buffer[256];
        BufferNumber(pList->array, pList->size, buffer, 256);
        char* start = buffer;
        char* end;
        double value = strtod(start, &end);
        if (start != end && !isnan(value) && !isinf(value)) {
            return value;
        }

        // maybe need to check overflow too

        if (start == end) {
            b.ErrorArgs(m, "double literal `%.*s` didn't parse to a value", pList->size, pList->array);
            return true;
        }
        else if (isnan(value)) {
            b.ErrorArgs(m, "double literal `%.*s` is NaN", pList->size, pList->array);
            return true;
        }
        else if (isinf(value)) {
            b.ErrorArgs(m, "double literal `%.*s` is infinite", pList->size, pList->array);
            return true;
        }
        else {
            b.ErrorArgs(m, "double literal `%.*s` is invalid", pList->size, pList->array);
            return true;
        }
    }

    float GetFloatValue(FixedPodList<char>* pList) {
        char buffer[256];
        BufferNumber(pList->array, pList->size, buffer, 256);
        char* start = buffer;
        char* end;
        float value = strtof(start, &end);

        if (start != end && !isnan(value) && !isinf(value)) {
            return value;
        }

        if (start == end) {
            b.ErrorArgs(m, "float literal `%.*s` didn't parse to a value", pList->size, pList->array);
            return 0;
        }
        else if (isnan(value)) {
            b.ErrorArgs(m, "float literal `%.*s` is NaN", pList->size, pList->array);
            return 0;
        }
        else if (isinf(value)) {
            b.ErrorArgs(m, "float literal `%.*s` is infinite", pList->size, pList->array);
            return 0;
        }
        else {
            b.ErrorArgs(m, "float literal `%.*s` is invalid", pList->size, pList->array);
            return 0;
        }

        return 0;
    }

    struct Diagnostics {
        LinearAllocator* allocator;
    };

    bool ScanNumericLiteral(TextWindow* textWindow, TokenInfo* info) {
        char* start = textWindow->ptr;
        char c;
        bool isHex = false;
        bool isBinary = false;
        bool hasDecimal = false;
        bool hasExponent = false;
        // _builder.Clear();
        bool hasUSuffix = false;
        bool hasLSuffix = false;
        bool underscoreInWrongPlace = false;
        bool usedUnderscore = false;
        bool firstCharWasUnderscore = false;

        TempAllocator* tempAllocator = GetThreadLocalAllocator();
        TempAllocator::ScopedMarker scope(tempAllocator);

        Alchemy::FixedPodList<char> buffer(tempAllocator->AllocateUncleared<char>(256), 256);

        c = textWindow->PeekChar();
        if (c == '0') {
            c = textWindow->PeekChar(1);
            if (c == 'x' || c == 'X') {
                textWindow->Advance(2);
                isHex = true;
            }
            else if (c == 'b' || c == 'B') {
                textWindow->Advance(2);
                isBinary = true;
            }
        }

        if (isHex || isBinary) {
            // It's OK if it has no digits after the '0x' -- we'll catch it in ScanNumericLiteral and give a proper error then.
            ScanNumericLiteralSingleInteger(textWindow, &buffer, &underscoreInWrongPlace, &usedUnderscore, &firstCharWasUnderscore, isHex, isBinary);

            if (textWindow->PeekChar('L') || textWindow->Peek('l')) {
                textWindow->Advance();
                hasLSuffix = true;
                if (textWindow->PeekChar('u') || textWindow->Peek('U')) {
                    textWindow->Advance();
                    hasUSuffix = true;
                }
            }
            else if (textWindow->PeekChar('u') || textWindow->Peek('U')) {
                textWindow->Advance();
                hasUSuffix = true;
                if (textWindow->PeekChar('L') || textWindow->Peek('l')) {
                    textWindow->Advance();
                    hasLSuffix = true;
                }
            }
        }
        else {
            ScanNumericLiteralSingleInteger(textWindow, &buffer, &underscoreInWrongPlace, &usedUnderscore, &firstCharWasUnderscore, false, false);

            c = textWindow->PeekChar();
            if (c == '.') {
                char next = textWindow->PeekChar(1);
                if (next >= '0' && next <= '9') {
                    hasDecimal = true;
                    buffer.Add(c);
                    textWindow->Advance();
                    ScanNumericLiteralSingleInteger(textWindow, &buffer, &underscoreInWrongPlace, &usedUnderscore, &firstCharWasUnderscore, false, false);
                }
                else if (buffer.size == 0) {
                    // we only have the dot so far.. (no preceding number or following number)
                    textWindow->ptr = start;
                    return false;
                }
            }
            c = textWindow->PeekChar();
            if (c == 'E' || c == 'e') {
                buffer.Add(c);
                textWindow->Advance();
                hasExponent = true;
                c = textWindow->PeekChar();
                if (c == '-' || c == '+') {
                    buffer.Add(c);
                    textWindow->Advance();
                }

                c = textWindow->PeekChar();
                if (!SyntaxFacts::IsDecDigit(c) && c != '_') {
                    AddError(ErrorCode::ERR_InvalidReal);
                    // add dummy exponent, so parser does not blow up
                    buffer.Add('0');
                }
                else {
                    ScanNumericLiteralSingleInteger(textWindow, &buffer, &underscoreInWrongPlace, &usedUnderscore, &firstCharWasUnderscore, false, false);
                }
            }
            c = textWindow->PeekChar();
            if (hasExponent || hasDecimal) {
                if (c == 'f' || c == 'F') {
                    textWindow->Advance();
                    info->valueKind = LiteralType::Float;
                }
                else if (c == 'D' || c == 'd') {
                    textWindow->Advance();
                    info->valueKind = LiteralType::Double;
                }
                else {
                    info->valueKind = LiteralType::Double;
                }
            }
            else if (c == 'f' || c == 'F') {
                textWindow->Advance();
                info->valueKind = LiteralType::Float;
            }
            else if (c == 'D' || c == 'd') {
                textWindow->Advance();
                info->valueKind = LiteralType::Double;
            }
            else if (c == 'L' || c == 'l') {
                textWindow->Advance();
                hasLSuffix = true;
                if (textWindow->Peek('u') || textWindow->Peek('U')) {
                    textWindow->Advance();
                    hasUSuffix = true;
                }
            }
            else if (c == 'u' || c == 'U') {
                hasUSuffix = true;
                textWindow->Advance();
                if (textWindow->Peek('L') || textWindow->Peek('l')) {
                    textWindow->Advance();
                    hasLSuffix = true;
                }
            }
        }

        if (underscoreInWrongPlace) {
            AddError(start, textWindow->ptr - start, ErrorCode::ERR_InvalidNumber);
        }

        info->Kind = SyntaxKind::NumericLiteralToken;
        info->text = FixedCharSpan(start, (int32) (textWindow->ptr - start));
        uint64 val;

        switch (info->valueKind) {
            case LiteralType::Float: {
                info->floatValue = GetFloatValue(&buffer);
                break;
            }
            case LiteralType::Double: {
                info->doubleValue = GetDoubleValue(&buffer);
                break;
            }
            default: {
                if (buffer.size == 0) {
                    if (!underscoreInWrongPlace) {
                        AddError(ErrorCode::ERR_InvalidNumber);
                    }
                    val = 0; //safe default
                }
                else {
                    val = GetValueUInt64(&buffer);
                }


                break;
            }
        }

    }

    bool IsIdentifierPartCharacter(uint32 codepoint) {
        // identifier-part-character:
        //   letter-character
        //   decimal-digit-character
        //   connecting-character
        //   combining-character
        //   formatting-character

        if ((codepoint >= 'A' && codepoint <= 'Z') || (codepoint >= 'a' && codepoint <= 'z') || (codepoint >= '0' && codepoint <= '9') || codepoint == '_' || Unicode::IsLetter(codepoint) || Unicode::IsNumber(codepoint)) {
            return true;
        }

        Unicode::UnicodeCategory category = Unicode::GetUnicodeCategory(codepoint);

        if (category == Unicode::UnicodeCategory::ConnectorPunctuation ||
            category == Unicode::UnicodeCategory::Format ||
            category == Unicode::UnicodeCategory::NonSpacingMark ||
            category == Unicode::UnicodeCategory::SpacingCombiningMark) {
            return true;
        }
        return false;
    }

    bool ScanIdentifier_SlowPath(TextWindow* textWindow, FixedCharSpan* identifier) {
        char* start = textWindow->ptr;

        char32 c;

        int32 advance = 0;
        if (!textWindow->TryPeekChar32(&c, &advance) || !IsIdentifierStartCharacter(c)) {
            return false;
        }

        textWindow->Advance(advance);

        while (textWindow->TryPeekChar32(&c, &advance)) {

            switch (c) {
                case '\\':
                case '$':
                case '&':
                case '\0':
                case ' ':
                case '\r':
                case '\n':
                case '\t':
                case '!':
                case '%':
                case '(':
                case ')':
                case '*':
                case '+':
                case ',':
                case '-':
                case '.':
                case '/':
                case ':':
                case ';':
                case '<':
                case '=':
                case '>':
                case '?':
                case '[':
                case ']':
                case '^':
                case '{':
                case '|':
                case '}':
                case '~':
                case '"': {
                    // all of the following characters are not valid in an identifier. If we see any of them, then we know we're done.
                    goto loop_exit;
                }
                default: {

                    if (IsIdentifierPartCharacter(c)) {
                        textWindow->Advance(advance);
                        continue;
                    }

                    goto loop_exit;

                }

            }

        }

        loop_exit:
        int32 length = (int32) (textWindow->ptr - start);
        *identifier = FixedCharSpan(start, length);
        return true;

    }

    bool IsNewline(char32 c) {
        // new-line-character:
        //   Carriage return character (U+000D)
        //   Line feed character (U+000A)
        //   Next line character (U+0085)
        //   Line separator character (U+2028)
        //   Paragraph separator character (U+2029)

        return c == U'\r'
               || c == U'\n'
               || c == U'\u0085'
               || c == U'\u2028'
               || c == U'\u2029';
    }

    void ScanSingleLineComment(TextWindow* textWindow, FixedCharSpan* span) {

        char* start = textWindow->ptr;

        if (textWindow->PeekChar() != '/' || textWindow->PeekChar(1) != '/') {
            *span = FixedCharSpan(textWindow->ptr, 0);
            return;
        }

        char32 c;
        int32 advance;
        while (textWindow->TryPeekChar32(&c, &advance)) {
            if (IsNewline(c)) {
                *span = FixedCharSpan(start, (int32) (textWindow->ptr - start));
                return;
            }
            textWindow->Advance(advance);
        }
    }

    void ScanMultiLineComment(TextWindow* textWindow, FixedCharSpan* span, bool* isTerminated) {
        char* start = textWindow->ptr;

        if (textWindow->PeekChar() != '/' || textWindow->PeekChar(1) != '*') {
            *span = FixedCharSpan(textWindow->ptr, 0);
            *isTerminated = true;
            return;
        }

        *isTerminated = false;
        textWindow->Advance(2);

        char32 c;
        int32 advance;
        while (textWindow->TryPeekChar32(&c, &advance)) {
            if (c == '*' && textWindow->PeekChar(1) == '/') {
                textWindow->Advance(2);
                *isTerminated = true;
                break;
            }
            else {
                textWindow->Advance(advance);
            }

        }

        *span = FixedCharSpan(start, (int32) (textWindow->ptr - start));

    }

    void ScanToEndOfLine(TextWindow* textWindow, FixedCharSpan* span) {
        char32 c;
        int32 advance;
        char* start = textWindow->ptr;
        while (textWindow->TryPeekChar32(&c, &advance)) {
            if (IsNewline(c)) {
                *span = FixedCharSpan(start, (int32) (textWindow->ptr - start));
                return;
            }
            textWindow->Advance(advance);
        }
    }

    void ScanWhitespace(TextWindow* textWindow, FixedCharSpan* whitespace) {

        char* start = textWindow->ptr;
        while (true) {

            char c = textWindow->PeekChar();
            if (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\u001A') {
                textWindow->Advance();
                continue;
            }

            char32 c32;
            int32 advance;

            if (textWindow->TryPeekChar32(&c32, &advance) && Unicode::GetUnicodeCategory(c32) == Unicode::UnicodeCategory::SpaceSeparator) {
                textWindow->Advance(advance);
                continue;
            }

            *whitespace = FixedCharSpan(start, (int32) (textWindow->ptr - start));
            return;
        }

    }

    bool ScanIdentifier_FastPath(TextWindow* textWindow, FixedCharSpan* identifier) {

        if (!textWindow->HasMoreContent()) {
            return false;
        }

        // localize because we don't want to advance unless we actually succeed
        char* start = textWindow->ptr;
        char* ptr = textWindow->ptr;
        char* end = textWindow->end;

        char current = *start;

        if (current >= '0' && current <= '9') {
            // if the first thing we see is a number, bail out
            return false;
        }

        while (ptr != end) {

            // no more characters, we're done here (and this is probably an invalid parse but doesn't matter at this point)

            current = *ptr;

            switch (current) {
                case '&':
                case '\0':
                case ' ':
                case '\r':
                case '\n':
                case '\t':
                case '!':
                case '%':
                case '(':
                case ')':
                case '*':
                case '+':
                case ',':
                case '-':
                case '.':
                case '/':
                case ':':
                case ';':
                case '<':
                case '=':
                case '>':
                case '?':
                case '[':
                case ']':
                case '^':
                case '{':
                case '|':
                case '}':
                case '~':
                case '"':
                case '\'': {
                    // all of these characters are not valid in an identifier. If we see any of them, then we know we're done.
                    goto end;
                }
                default: {
                    // legal identifier characters
                    if ((current >= 'a' && current <= 'z') || (current >= 'A' && current <= 'Z') || (current >= '0' && current <= '9') || current == '_') {
                        ptr++;
                        continue;
                    }

                    // anything else we bail out on and we'll try the slow path
                    return false;
                }
            }


        }
        end:
        if (ptr != start) {
            int32 length = (int32) (ptr - start);
            textWindow->Advance(length);
            *identifier = FixedCharSpan(start, length);
            return true;
        }

        return false;

    }

    bool ScanIdentifier(TextWindow* textWindow, FixedCharSpan* fixedCharSpan) {
        return ScanIdentifier_FastPath(textWindow, fixedCharSpan) || ScanIdentifier_SlowPath(textWindow, fixedCharSpan);
    }

}