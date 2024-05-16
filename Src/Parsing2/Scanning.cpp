#include "./TextWindow.h"
#include "./SyntaxFacts.h"
#include "../Util/FixedCharSpan.h"
#include "../Unicode/Unicode.h"
#include "./Scanning.h"
#include "../Collections/FixedPodList.h"
#include "../Allocation/ThreadLocalTemp.h"

namespace Alchemy::Compilation {

    Diagnostic CreateIllegalEscapeDiagnostic(TextWindow* window, char* start) {
        return Diagnostic(ErrorCode::ERR_IllegalEscape, start, window->ptr);
    }

    uint32 ScanUnicodeEscape(TextWindow* textWindow, Diagnostic* error) {
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
            else if (!(isHex
                       ? SyntaxFacts::IsHexDigit(ch)
                       : isBinary
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


    bool TryParseHexUInt64(char * src, int32 length, uint64* value) {
        *value = 0;
        // can't fit into the value, overflow
        if (length > 16) {
            *value = 0;
            return false;
        }

        char buffer[17];
        memcpy(buffer, src, length);
        buffer[length] = '\0';

        // we've already lexed valid hex
        char* end;
        errno = 0;
        *value = strtoull(buffer, &end, 16);

        return true;
    }

    bool TryParseBinaryUInt64(char * src, int32 length, uint64* value) {
        *value = 0;
        for (int32 i = 0; i < length; i++) {
            // if uppermost bit is set, then the next bitshift will overflow
            if ((*value & 0x8000000000000000) != 0) {
                *value = 0;
                return false;
            }
            // We shouldn't ever get a string that's nonbinary (see ScanNumericLiteral),
            // so don't explicitly check for it (there's a debug assert in SyntaxFacts)
            uint64 bit = src[i] == '1' ? 1 : 0;
            *value = (*value << 1ull) | bit;
        }

        return true;
    }


    bool TryGetValueUInt64(char* src, int32 length, uint64* value) {
        char buffer[128];
        BufferNumber(src, length, buffer, 128);
        char* end;
        errno = 0;
        *value = strtoull(buffer, &end, 10);
        if ((errno == ERANGE && *value == ULLONG_MAX) || (errno != 0 && *value == 0)) {
            *value = 0;
            return false;
        }
        return true;
    }

    int64 GetInt64Value(char* str, int32 length) {
        uint64 retn = 0;
        TryGetValueUInt64(str, length, &retn);
        return (int64) retn;
    }

    uint32 GetUInt32Value(char* str, int32 length) {
        uint64 retn = 0;
        TryGetValueUInt64(str, length, &retn);
        return (uint32) retn;
    }

    uint64 GetBinaryValue(char* str, int32 length) {
        uint64 value = 0;
        char buffer[65];
        if(str[0] == '0' && str[1] == 'b') {
            str += 2;
            length -=2;
        }

        int32 cnt = 0;
        for(int32 i = 0; i < length; i++) {
            if(str[i] == '_') continue;
            if(str[i] != '1' && str[i] != '0') {
                break;
            }
            buffer[cnt++] = str[i];
        }
        buffer[cnt] = '\0';
        TryParseBinaryUInt64(str, length, &value);
        return value;
    }

    uint64 GetHexValue(char* str, int32 length) {
        uint64 value = 0;
        if(str[0] == '0' && str[1] == 'x') {
            str += 2;
            length -= 2;
        }

        char buffer[17];
        int32 cnt = 0;

        for(int32 i = 0; i < length; i++) {
            if(str[i] == '_') continue;
            if(!SyntaxFacts::IsHexDigit(str[i])){
                break;
            }
            buffer[cnt++] = str[i];
        }
        buffer[cnt] = '\0';
        TryParseHexUInt64(buffer, cnt, &value);
        return value;
    }

    int32 GetInt32Value(char* str, int32 length) {
        uint64 retn = 0;
        TryGetValueUInt64(str, length, &retn);
        return (int32) retn;
    }

    uint64 GetUInt64Value(char* str, int32 length) {
        uint64 retn = 0;
        TryGetValueUInt64(str, length, &retn);
        return retn;
    }

    bool TryGetDoubleValue(FixedPodList<char>* pList, double* pValue, ErrorCode* errorCode) {
        char buffer[256];
        BufferNumber(pList->array, pList->size, buffer, 256);
        char* start = buffer;
        char* end;
        double value = strtod(start, &end);

        if (start != end && !isnan(value) && !isinf(value)) {
            *pValue = value;
            return true;
        }

        if (start == end) {
            *errorCode = ErrorCode::ERR_InvalidReal;
            *pValue = 0;
            return false;
        }
        else if (isnan(value)) {
            *errorCode = ErrorCode::ERR_InvalidRealNaN;
            *pValue = 0;
            return false;
        }
        else if (isinf(value)) {
            *errorCode = ErrorCode::ERR_InvalidRealInfinite;
            *pValue = 0;
            return false;
        }
        else {
            *errorCode = ErrorCode::ERR_InvalidReal;
            *pValue = 0;
            return false;
        }
    }

    bool TryGetFloatValue(FixedPodList<char>* pList, float* pValue, ErrorCode* errorCode) {
        char buffer[256];
        BufferNumber(pList->array, pList->size, buffer, 256);
        char* start = buffer;
        char* end = nullptr;
        float value = strtof(start, &end);

        if (start != end && !isnan(value) && !isinf(value)) {
            *pValue = value;
            return true;
        }

        if (start == end) {
            *errorCode = ErrorCode::ERR_InvalidReal;
            *pValue = 0;
            return false;
        }
        else if (isnan(value)) {
            *errorCode = ErrorCode::ERR_InvalidRealNaN;
            *pValue = 0;
            return false;
        }
        else if (isinf(value)) {
            *errorCode = ErrorCode::ERR_InvalidRealInfinite;
            *pValue = 0;
            return false;
        }
        else {
            *errorCode = ErrorCode::ERR_InvalidReal;
            *pValue = 0;
            return false;
        }

    }

    float GetFloatValue(char* str, int32 length) {
        char buffer[128];
        BufferNumber(str, length, buffer, 128);
        char* start = buffer;
        char* end = nullptr;
        float value = strtof(start, &end);

        if (start != end && !isnan(value) && !isinf(value)) {
            return value;
        }

        return 0;

    }

    // todo -- maybe move value parsing to the parser, just figure out the type here and don't report errors
    bool ScanNumericLiteral(TextWindow* textWindow, Diagnostics* diagnostics, SyntaxToken* info) {
        char* start = textWindow->ptr;
        char c;
        bool isHex = false;
        bool isBinary = false;
        bool hasDecimal = false;
        bool hasExponent = false;
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
            c = textWindow->PeekAhead(1);
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

            if (textWindow->Peek('L') || textWindow->Peek('l')) {
                textWindow->Advance();
                hasLSuffix = true;
                if (textWindow->Peek('u') || textWindow->Peek('U')) {
                    textWindow->Advance();
                    hasUSuffix = true;
                }
            }
            else if (textWindow->Peek('u') || textWindow->Peek('U')) {
                textWindow->Advance();
                hasUSuffix = true;
                if (textWindow->Peek('L') || textWindow->Peek('l')) {
                    textWindow->Advance();
                    hasLSuffix = true;
                }
            }
        }
        else {
            ScanNumericLiteralSingleInteger(textWindow, &buffer, &underscoreInWrongPlace, &usedUnderscore, &firstCharWasUnderscore, false, false);

            c = textWindow->PeekChar();
            if (c == '.') {
                char next = textWindow->PeekAhead(1);
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
                    diagnostics->AddError(Diagnostic(ErrorCode::ERR_InvalidReal, start, textWindow->ptr));
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
                    info->contextualKind = TokenKind::FloatLiteral;
                }
                else if (c == 'D' || c == 'd') {
                    textWindow->Advance();
                    info->contextualKind = TokenKind::DoubleLiteral;
                }
                else {
                    info->contextualKind = TokenKind::DoubleLiteral;
                }
            }
            else if (c == 'f' || c == 'F') {
                textWindow->Advance();
                info->contextualKind = TokenKind::FloatLiteral;
            }
            else if (c == 'D' || c == 'd') {
                textWindow->Advance();
                info->contextualKind = TokenKind::DoubleLiteral;
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
            diagnostics->AddError(Diagnostic(ErrorCode::ERR_InvalidNumber, start, textWindow->ptr));
        }

        info->kind = TokenKind::NumericLiteralToken;
        info->text = start;
        info->textSize = (int32) ((textWindow->ptr - start));
        uint64 val;

        switch (info->contextualKind) {
            case TokenKind::FloatLiteral: {
                ErrorCode errorCode;
                float floatValue = 0;
                if (!TryGetFloatValue(&buffer, &floatValue, &errorCode)) {
                    diagnostics->AddError(Diagnostic(errorCode, start, textWindow->ptr));
                }
                break;
            }
            case TokenKind::DoubleLiteral: {
                ErrorCode errorCode;
                double doubleValue = 0;
                if (!TryGetDoubleValue(&buffer, &doubleValue, &errorCode)) {
                    diagnostics->AddError(Diagnostic(errorCode, start, textWindow->ptr));
                }
                break;
            }
            default: {
                if (buffer.size == 0) {
                    if (!underscoreInWrongPlace) {
                        diagnostics->AddError(Diagnostic(ErrorCode::ERR_InvalidNumber, start, textWindow->ptr));
                    }
                    val = 0; //safe default
                }
                else {
                    if (isBinary) {
                        if (!TryParseBinaryUInt64(buffer.array, buffer.size, &val)) {
                            // overflow is the only error possible
                            diagnostics->AddError(Diagnostic(ErrorCode::ERR_IntOverflow, start, textWindow->ptr));
                        }
                    }
                    else if (isHex) {
                        if (!TryParseHexUInt64(buffer.array, buffer.size, &val)) {
                            // overflow is the only error possible
                            diagnostics->AddError(Diagnostic(ErrorCode::ERR_IntOverflow, start, textWindow->ptr));
                        }
                    }
                    else {
                        if (!TryGetValueUInt64(buffer.array, buffer.size, &val)) {
                            diagnostics->AddError(Diagnostic(ErrorCode::ERR_IntOverflow, start, textWindow->ptr));
                        }
                    }
                }

                // If the literal has no suffix, it has the first of these types in which its value can be represented: int, uint, long, ulong.

                if (!hasUSuffix && !hasLSuffix) {
                    if (val <= std::numeric_limits<int32>::max()) {
                        info->contextualKind = TokenKind::Int32Literal;
//                        info->literalValue.int32Value = (int32) val;
                    }
                    else if (val <= std::numeric_limits<uint32>::max()) {
                        info->contextualKind = TokenKind::UInt32Literal;
//                        info->literalValue.uint32Value = (uint32) val;
                    }
                    else if (val <= std::numeric_limits<int64>::max()) {
                        info->contextualKind = TokenKind::Int64Literal;
//                        info->literalValue.int64Value = (int64) val;
                    }
                    else {
                        info->contextualKind = TokenKind::UInt64Literal;
//                        info->literalValue.uint64Value = val;
                    }
                }
                else if (hasUSuffix && !hasLSuffix) {
                    // * If the literal is suffixed by U or u, it has the first of these types in which its value can be represented: uint, ulong.
                    if (val <= std::numeric_limits<uint32>::max()) {
                        info->contextualKind = TokenKind::UInt32Literal;
//                        info->literalValue.uint32Value = (uint32) val;
                    }
                    else {
                        info->contextualKind = TokenKind::UInt64Literal;
//                        info->literalValue.uint64Value = val;
                    }
                }

                    // * If the literal is suffixed by L or l, it has the first of these types in which its value can be represented: long, ulong.
                else if (!hasUSuffix & hasLSuffix) {
                    if (val <= std::numeric_limits<int64>::max()) {
                        info->contextualKind = TokenKind::Int64Literal;
//                        info->literalValue.int64Value = (int64) val;
                    }
                    else {
                        info->contextualKind = TokenKind::UInt64Literal;
//                        info->literalValue.uint64Value = val;
                    }
                }

                    // * If the literal is suffixed by UL, Ul, uL, ul, LU, Lu, lU, or lu, it is of type ulong.
                else {
                    assert(hasUSuffix && hasLSuffix);
                    info->contextualKind = TokenKind::UInt64Literal;
//                    info->literalValue.uint64Value = val;
                }
                break;
            }
        }

        return true;

    }

    bool ScanIdentifierOrKeyword(TextWindow* textWindow, SyntaxToken* info) {

        FixedCharSpan identifier;

        if (ScanIdentifier_FastPath(textWindow, &identifier)) {
            info->text = identifier.ptr;
            info->textSize = identifier.size;
            // check if it's a keyword (no need to try if fast path didn't work)
            TokenKind keyword;
            if (TryMatchKeyword_Generated(identifier.ptr, identifier.size, &keyword)) {

                if (SyntaxFacts::IsReservedKeyword(keyword)) {
                    info->kind = keyword;
                    info->contextualKind = TokenKind::None;
                }
                else {
                    // it's contextual
                    assert(SyntaxFacts::IsContextualKeyword(keyword));
                    info->kind = TokenKind::IdentifierToken;
                    info->contextualKind = keyword;

                }

            }
            else {
                // not a keyword
                info->kind = TokenKind::IdentifierToken;
                info->contextualKind = TokenKind::IdentifierToken;
            }

            return true;

        }
        else if (ScanIdentifier_SlowPath(textWindow, &identifier)) {
            info->text = identifier.ptr;
            info->textSize = identifier.size;
            info->kind = TokenKind::IdentifierToken;
            info->contextualKind = TokenKind::IdentifierToken;
            return true;
        }
        else {
            info->kind = TokenKind::None;
            info->contextualKind = TokenKind::None;
            return false;
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

        if (textWindow->PeekChar() != '/' || textWindow->PeekAhead(1) != '/') {
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

        if (textWindow->PeekChar() != '/' || textWindow->PeekAhead(1) != '*') {
            *span = FixedCharSpan(textWindow->ptr, 0);
            *isTerminated = true;
            return;
        }

        *isTerminated = false;
        textWindow->Advance(2);

        char32 c;
        int32 advance;
        while (textWindow->TryPeekChar32(&c, &advance)) {
            if (c == '*' && textWindow->PeekAhead(1) == '/') {
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
                textWindow->Advance(advance);
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