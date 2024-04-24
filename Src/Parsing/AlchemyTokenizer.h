#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/PodList.h"
#include "../Unicode/Unicode.h"
#include "./Keyword.h"
#include "./Token.h"
#include "MatchKeyword_Generated2.h"
#include "../Collections/FixedPodList.h"

using namespace Alchemy;

char32 PeekUtf32_FromUtf8(const char* buffer, int32* byteWidth) {
    char32 result = 0;

    // Check the number of bytes in the current UTF-8 char based on the first byte
    if ((*buffer & 0x80) == 0) {           // 0xxxxxxx
        result = *buffer;
        *byteWidth = 1;
    }
    else if ((*buffer & 0xE0) == 0xC0) {  // 110xxxxx
        result = (*buffer & 0x1F) << 6;
        result |= (buffer[1] & 0x3F);
        *byteWidth = 2;
    }
    else if ((*buffer & 0xF0) == 0xE0) {  // 1110xxxx
        result = (*buffer & 0x0F) << 12;
        result |= (buffer[1] & 0x3F) << 6;
        result |= (buffer[2] & 0x3F);
        *byteWidth = 3;
    }
    else if ((*buffer & 0xF8) == 0xF0) {  // 11110xxx
        result = (*buffer & 0x07) << 18;
        result |= (buffer[1] & 0x3F) << 12;
        result |= (buffer[2] & 0x3F) << 6;
        result |= (buffer[3] & 0x3F);
        *byteWidth = 4;
    }
    else {
        *byteWidth = 0;
    }
    return result;
}

namespace Alchemy {

    struct Tokenizer {

        bool Tokenize(const char* buffer_, int32 length_, Alchemy::PodList<Token>* outputs) {
            State states[kStateStackLimit];
            FixedPodList<State> stateBuffer(states, kStateStackLimit);
            return PerformTokenize((char*) buffer_, length_, &stateBuffer, outputs);
        }

        bool Tokenize(char* buffer_, int32 length_, Alchemy::PodList<Token>* outputs) {
            State states[kStateStackLimit];
            FixedPodList<State> stateBuffer(states, kStateStackLimit);
            return PerformTokenize(buffer_, length_, &stateBuffer, outputs);
        }

    private:

        static constexpr int32 kStateStackLimit = 128;

        enum class TokenizerState : uint16 {
            Default,
            String,
            Style,
            Character,
            MultiLineString,
            StringInterpolation
        };

        struct State {

            uint16 leftBraceCount {};
            TokenizerState state {};

            State() = default;

            State(TokenizerState state, uint16 leftBraceCount)
                : leftBraceCount(leftBraceCount)
                , state(state) {}

        };

        int32 length;
        char* buffer;
        char* ptr;
        char* end;

        TokenizerState state;

        int32 leftBraceCount;

        FixedPodList<State>* stateStack;

        bool HasMoreInput() {
            return ptr != end;
        }

        char32 GetCharUnchecked(int32* advance) {
            return PeekUtf32_FromUtf8(ptr, advance);
        }

        bool TryPeekChar(char* c) {
            if (ptr != end) {
                *c = *ptr;
                return true;
            }
            return false;
        }

        int32 GetLocation() {
            return int32(ptr - buffer);
        }

        char Peek(int32 lookAhead) {

            if (ptr + lookAhead < end) {
                return *(ptr + lookAhead);
            }

            return 0;
        }

        bool TryPeek32(char32* c, int32* advance) {
            if (HasMoreInput()) {
                *c = PeekUtf32_FromUtf8(ptr, advance);
                return true;
            }
            return false;
        }

        bool TryReadWhitespace(Token* token) {
            // : [\p{Zs}]  // any character with Unicode class Zs
            // | '\u0009'  // horizontal tab
            // | '\u000B'  // vertical tab
            // | '\u000C'  // form feed

            int32 startLocation = GetLocation();

            while (HasMoreInput()) {
                char c = *ptr;

                if (c == ' ' || c == '\u0009' || c == '\u000B' || c == '\u000C') {
                    ptr++;
                    continue;
                }

                if(c == '\n' || c == '\r') {
                    ptr++;
                    continue;
                }

                // multi byte character
                if ((c & 0x80) != 0) {
                    char32 c32;
                    int32 advance;
                    if (TryPeek32(&c32, &advance) && Unicode::GetUnicodeCategory(c32) == Unicode::UnicodeCategory::SpaceSeparator) {
                        ptr += advance;
                        continue;
                    }
                }

                break;
            }

            int32 endLocation = GetLocation();
            if (startLocation != endLocation) {
                *token = Token(TokenType::Whitespace, startLocation);
                return true;
            }

            return false;
        }

        bool TryReadComment(Token* token) {

            if (!HasMoreInput()) {
                return false;
            }

            char c = *ptr;
            if (c != '/') {
                return false;
            }

            int32 startLocation = GetLocation();

            ptr++;

            if (!HasMoreInput()) {
                ptr--;
                return false;
            }

            c = *ptr;
            if (c == '/') {
                ptr++;
                char32 c32;
                int32 advance;
                // read until new line or end of file
                while (TryPeek32(&c32, &advance)) {
                    ptr += advance;
                    if (Unicode::IsNewLine(c32)) { // I guess we can decide if the new line is part of the comment or part of a whitespace
                        break;
                    }
                }
                *token = Token(TokenType::Comment, startLocation);
                return true;
            }
            else {
                ptr--;
                return false;
            }

        }

        void MakeSymbol(Token* token, TokenType tokenType, int32 tokenLength) {
            *token = Token(tokenType, GetLocation() - tokenLength);
        }

        bool TryMatchOperator(Token* token) {

            // read until not a symbol character (how do i know that?)
            // if multibyte
            // peek a byte check if it's a symbol
            // if it is, peek the next byte and check if symbol

            char c;

            if (!TryPeekChar(&c)) {
                return false;
            }

            switch (c) {

                case '@': {
                    ptr++;
                    MakeSymbol(token, TokenType::At, 1);
                    return true;
                }

                case '&': {
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '&') {
                            ptr++;
                            MakeSymbol(token, TokenType::ConditionalAnd, 2);
                            return true;
                        }
                        else if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::AndAssign, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::BinaryAnd, 1);
                    return true;
                }

                case '|': {
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '|') {
                            ptr++;
                            MakeSymbol(token, TokenType::ConditionalOr, 2);
                            return true;
                        }
                        else if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::OrAssign, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::BinaryOr, 1);
                    return true;
                }
                case '^': {

                    ptr++;
                    if (TryPeekChar(&c) && c == '=') {
                        ptr++;
                        MakeSymbol(token, TokenType::XorAssign, 2);
                        return true;
                    }
                    MakeSymbol(token, TokenType::BinaryXor, 1);
                    return true;
                }
                case '?': {
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '?') {
                            ptr++;
                            if (TryPeekChar(&c) && c == '=') {
                                ptr++;
                                MakeSymbol(token, TokenType::CoalesceAssign, 3);
                                return true;
                            }
                            MakeSymbol(token, TokenType::Coalesce, 2);
                            return true;
                        }
                        else if (c == '.') {
                            ptr++;
                            MakeSymbol(token, TokenType::ConditionalAccess, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::QuestionMark, 1);
                    return true;
                }
                case '=': {
                    // => FatArrow
                    // == Equals
                    // =  Assign
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '>') {
                            ptr++;
                            MakeSymbol(token, TokenType::FatArrow, 2);
                            return true;
                        }
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::ConditionalEquals, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Assign, 1);
                    return true;
                }
                case '!': {
                    ptr++;
                    if (TryPeekChar(&c) && c == '=') {
                        ptr++;
                        MakeSymbol(token, TokenType::ConditionalNotEquals, 2);
                        return true;
                    }
                    MakeSymbol(token, TokenType::Not, 1);
                    return true;
                }
                case '+': {
                    // +
                    // +=
                    // ++
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::AddAssign, 2);
                            return true;
                        }
                        if (c == '+') {
                            ptr++;
                            MakeSymbol(token, TokenType::Increment, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Plus, 1);
                    return true;
                }
                case '-': {
                    // -
                    // -=
                    // --
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::SubtractAssign, 2);
                            return true;
                        }
                        if (c == '-') {
                            ptr++;
                            MakeSymbol(token, TokenType::Decrement, 2);
                            return true;
                        }
                        if (c == '>') {
                            ptr++;
                            MakeSymbol(token, TokenType::ThinArrow, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Minus, 1);
                    return true;
                }
                case '*' : {
                    // *
                    // *=
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::MultiplyAssign, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Multiply, 1);
                    return true;
                }
                case '/' : {
                    // /
                    // /=
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::DivideAssign, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Divide, 1);
                    return true;
                }
                case '%' : {
                    // %
                    // %=
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::ModAssign, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Modulus, 1);
                    return true;
                }
                case ':': {
                    // :
                    // ::
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == ':') {
                            ptr++;
                            MakeSymbol(token, TokenType::DoubleColon, 2);
                            return true;
                        }
                    }
                    MakeSymbol(token, TokenType::Colon, 1);
                    return true;
                }
                case '<': {
                    // <
                    // <=
                    // <<=
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::LessThanEqualTo, 2);
                            return true;
                        }
                        // we don't take << as a token because of generics
                        if (c == '<') {
                            ptr++;
                            if (TryPeekChar(&c) && c == '=') {
                                ptr++;
                                MakeSymbol(token, TokenType::LeftShiftAssign, 3);
                                return true;
                            }
                            else {
                                ptr--;
                            }
                        }
                    }
                    MakeSymbol(token, TokenType::AngleBracketOpen, 1);
                    return true;
                }
                case '>': {
                    // >
                    // >=
                    // >>=
                    ptr++;
                    if (TryPeekChar(&c)) {
                        if (c == '=') {
                            ptr++;
                            MakeSymbol(token, TokenType::GreaterThanEqualTo, 2);
                            return true;
                        }
                        //
                        // we don't take << as a token because of generics
                        if (c == '>') {
                            ptr++;
                            if (TryPeekChar(&c) && c == '=') {
                                ptr++;
                                MakeSymbol(token, TokenType::RightShiftAssign, 3);
                                return true;
                            }
                            ptr--;
                        }
                    }
                    MakeSymbol(token, TokenType::AngleBracketClose, 1);
                    return true;
                }
                case '~': {
                    ptr++;
                    MakeSymbol(token, TokenType::BinaryNot, 1);
                    return true;
                }
                case '.': {
                    ptr++;
                    if (TryPeekChar(&c) && c == '.') {
                        ptr++;
                        if (TryPeekChar(&c) && c == '.') {
                            ptr++;
                            MakeSymbol(token, TokenType::Splat, 3);
                            return true;
                        }
                        else {
                            ptr--;
                        }
                    }
                    MakeSymbol(token, TokenType::Dot, 1);
                    return true;
                }
                case ';': {
                    ptr++;
                    MakeSymbol(token, TokenType::SemiColon, 1);
                    return true;
                }
                case '{': {
                    ptr++;
                    MakeSymbol(token, TokenType::CurlyBraceOpen, 1);
                    return true;
                }
                case '}': {
                    ptr++;
                    MakeSymbol(token, TokenType::CurlyBraceClose, 1);
                    return true;
                }
                case '[': {
                    ptr++;
                    MakeSymbol(token, TokenType::SquareBraceOpen, 1);
                    return true;
                }
                case ']': {
                    ptr++;
                    MakeSymbol(token, TokenType::SquareBraceClose, 1);
                    return true;
                }
                case '(': {
                    ptr++;
                    MakeSymbol(token, TokenType::OpenParen, 1);
                    return true;
                }
                case ')': {
                    ptr++;
                    MakeSymbol(token, TokenType::CloseParen, 1);
                    return true;
                }
                case '#': {
                    ptr++;
                    MakeSymbol(token, TokenType::HashTag, 1);
                    return true;
                }
                case ',': {
                    ptr++;
                    MakeSymbol(token, TokenType::Comma, 1);
                    return true;
                }
                default: {
                    return false;
                }
            }

        }

        bool PeekStr(const char* c, int32 strLen) {
            if (ptr + strLen >= end) {
                return false;
            }
            for (int i = 0; i < strLen; i++) {
                if (ptr[i] != c[i]) {
                    return false;
                }
            }
            return true;
        }

        TokenType ReadIntegerSuffix() {

            TokenType suffixType = TokenType::Int32Literal;

            char c;
            if (!TryPeekChar(&c)) {
                return suffixType;
            }

            if (c == 'u' || c == 'U') {
                ptr++;
                suffixType = TokenType::UInt32Literal;
                if (!TryPeekChar(&c) || (c != 'l' && c != 'L')) {
                    return suffixType;
                }
                ptr++;
                suffixType = TokenType::UInt64Literal;
            }
            else if (c == 'l' || c == 'L') {
                ptr++;
                suffixType = TokenType::Int64Literal;
            }

            return suffixType;
        }

        bool TryReadIntegerLiteral(Token* token) {

            int32 startLocation = GetLocation();

            char32 c32;
            int32 advance;

            if (!TryPeek32(&c32, &advance) || !Unicode::IsDigit(c32)) {
                return false;
            }

            ptr += advance;

            bool isZero = c32 == U'0';

            if (isZero) {
                // we look for binary & hex numbers
                if (TryPeek32(&c32, &advance)) {

                    if (c32 == U'b' || c32 == U'B') {
                        ptr += advance;
                        // we probably need to make sure we have a digit following the 0b, I think the parser is responsible for that
                        // parser also needs to validate storage class (too many bits)
                        while (TryPeek32(&c32, &advance)) {
                            if (c32 != '0' && c32 != '1' && c32 != '_') {
                                break;
                            }
                            ptr += advance;
                        }
                        *token = Token(TokenType::BinaryNumberLiteral, startLocation);
                        return true;

                    }
                    else if (c32 == U'x' || c32 == U'X') {
                        ptr += advance;
                        while (TryPeek32(&c32, &advance)) {
                            if (!Unicode::IsHexDigit(c32)) {
                                break;
                            }
                            ptr += advance;
                        }
                        *token = Token(TokenType::HexLiteral, startLocation);
                        return true;
                    }

                }
            }

            // keep reading until we hit something that isn't a digit or _
            while (TryPeek32(&c32, &advance)) {

                if (!Unicode::IsDigit(c32) && c32 != '_') {
                    break;
                }

                ptr += advance;

            }

            // don't allow trailing _
            char last = ptr[-1];
            if (last == '_') {
                ptr--;
            }

            *token = Token(ReadIntegerSuffix(), startLocation);
            return true;

        }

        bool ReadDigits() {

            int32 read = 0;
            char32 c;
            int32 advance;
            // keep reading until we hit something that isn't a digit
            while (TryPeek32(&c, &advance)) {
                if (!Unicode::IsDigit(c)) {
                    break;
                }
                ptr += advance;
                read++;
            }
            return read > 0;
        }

        bool TryReadRealLiteralSuffix(TokenType* literalType) {
            //  real_type_suffix_opt:
            //    (none) defaults to float
            //    F f D d

            char32 c;
            int32 advance;
            if (!TryPeek32(&c, &advance)) {
                return false;
            }

            if (c == 'f' || c == 'F') {
                ptr += advance;
                *literalType = TokenType::FloatLiteral;
                return true;
            }

            if (c == 'd' || c == 'D') {
                ptr += advance;
                *literalType = TokenType::DoubleLiteral;
                return true;
            }

            return false;
        }

        bool TryReadRealLiteral(Token* token) {
            int32 startLocation = GetLocation();

            if (!ReadDigits()) {
                return false;
            }

            char32 c;
            int32 advance;
            if (!TryPeek32(&c, &advance)) {
                // need a dot, an 'e', or a suffix to count as a real
                return false;
            }

            TokenType literalType = TokenType::FloatLiteral;

            if (c == '.') {
                ptr += advance;
                if (!ReadDigits()) {
                    return false;
                }

                TryReadRealLiteralSuffix(&literalType);
                *token = Token(literalType, startLocation);
                return true;
            }
            else if (c == 'e' || c == 'E') {
                ptr += advance;

                if (!TryPeek32(&c, &advance)) {
                    return false;
                }

                if (c == '+' || c == '-') {
                    ptr += advance;
                }

                if (!ReadDigits()) {
                    return false;
                }

                TryReadRealLiteralSuffix(&literalType);
                *token = Token(literalType, startLocation);
                return true;

            }
            else if (TryReadRealLiteralSuffix(&literalType)) {
                *token = Token(literalType, startLocation);
                return true;
            }
            else {
                return false;
            }
        }

        bool TryReadNumber(Token* token) {

            char32 c;
            int32 advance;
            char* startPtr = ptr;

            int32 startLocation = GetLocation();

            if (!TryPeek32(&c, &advance) || !Unicode::IsDigit(c)) {
                return false;
            }

            Token integerToken;
            Token realToken;

            bool validInteger = TryReadIntegerLiteral(&integerToken);
            char* intPtr = ptr;
            ptr = startPtr;
            bool validReal = TryReadRealLiteral(&realToken);
            char* realPtr = ptr;

            ptrdiff_t diff = intPtr - realPtr;

            if (validInteger && validReal) {

                // take the longer one
                if (diff > 0) {
                    ptr = intPtr;
                    *token = Token(integerToken.tokenType, startLocation);
                    return true;
                }
                else {
                    ptr = realPtr;
                    *token = Token(realToken.tokenType, startLocation);
                    return true;
                }
            }
            else if (validReal) {
                ptr = realPtr;
                *token = Token(realToken.tokenType, startLocation);
                return true;
            }
            else if (validInteger) {
                ptr = intPtr;
                *token = Token(integerToken.tokenType, startLocation);

                return true;
            }
            else {
                ptr = startPtr;
                return false;
            }

        }

        bool TryReadIdentifierOrKeyword(Token* token) {

            char32 c32;
            int32 advance;
            int32 startLocation = GetLocation();
            char* startPtr = ptr;

            if (!TryPeek32(&c32, &advance) || !Unicode::IsLetter(c32) && c32 != U'_') {
                return false;
            }

            ptr += advance;
            // fragment Identifier_Part_Character
            //     : Letter_Character
            //     | Decimal_Digit_Character
            //     | Connecting_Character -> Pc category
            //     | Combining_Character -> Mn or Mc categories
            //     | Formatting_Character
            //     ;
            while (TryPeek32(&c32, &advance)) {

                if (Unicode::IsLetterOrDigit(c32)) {
                    ptr += advance;
                    continue;
                }

                Unicode::UnicodeCategory category = Unicode::GetUnicodeCategory(c32);
                if (category == Unicode::UnicodeCategory::ConnectorPunctuation ||
                    category == Unicode::UnicodeCategory::Format ||
                    category == Unicode::UnicodeCategory::NonSpacingMark ||
                    category == Unicode::UnicodeCategory::SpacingCombiningMark) {
                    ptr += advance;
                    continue;
                }

                break;
            }

            int32 endLocation = GetLocation();

            Keyword keyword;
            if (TryMatchKeyword_Generated(startPtr, endLocation - startLocation, &keyword)) {
                *token = Token(TokenType::KeywordOrIdentifier, keyword, startLocation);
            }
            else {
                *token = Token(TokenType::KeywordOrIdentifier, startLocation);
            }

            return true;

        }

        void PushState(TokenizerState _state) {
            stateStack->Push(State(state, leftBraceCount));
            state = _state;
            leftBraceCount = 0;
        }

        void PopState() {
            assert(stateStack->size > 0 && "state stack should not be empty but it is");
            State _state = stateStack->Pop();
            this->state = _state.state;
            this->leftBraceCount = _state.leftBraceCount;
        }

        bool PerformTokenize(char* buffer_, int32 length_, FixedPodList<State>* stateStack_, Alchemy::PodList<Token>* outputs) {

            this->stateStack = stateStack_;
            this->leftBraceCount = 0;
            this->buffer = buffer_;
            this->length = length_;
            this->ptr = buffer;
            this->end = buffer + length;
            this->state = TokenizerState::Default;

            Token* token = outputs->Reserve();

            bool hasBadCharacters = false;

            loop:
            while (HasMoreInput()) {

                switch (state) {

                    case TokenizerState::Default: {

                        bool whitespaceComment = true;

                        while (whitespaceComment) {

                            whitespaceComment = false;

                            if (TryReadComment(token)) {
                                token = outputs->Reserve();
                                whitespaceComment = true;
                            }

                            if (TryReadWhitespace(token)) {
                                token = outputs->Reserve();
                                whitespaceComment = true;
                            }

                        }

                        if (!HasMoreInput()) {
                            break;
                        }

                        if (*ptr == '"') {

                            if (PeekStr(R"(""")", 3)) {
                                *token = Token(TokenType::MultiLineStringStart, GetLocation());
                                token = outputs->Reserve();
                                ptr += 3;
                                PushState(TokenizerState::MultiLineString);
                                goto loop;
                            }
                            else {
                                *token = Token(TokenType::StringStart, GetLocation());
                                token = outputs->Reserve();
                                ptr++;
                                PushState(TokenizerState::String);
                                goto loop;
                            }

                        }

                        if (*ptr == '`') {
                            *token = Token(TokenType::OpenStyle, GetLocation());
                            token = outputs->Reserve();
                            ptr++;
                            PushState(TokenizerState::Style);
                            goto loop;
                        }

                        if (*ptr == '\'') {
                            *token = Token(TokenType::OpenCharacter, GetLocation());
                            token = outputs->Reserve();
                            ptr++;
                            PushState(TokenizerState::Character);
                            goto loop;
                        }

                        if (TryReadIdentifierOrKeyword(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        if (TryReadNumber(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        if (TryMatchOperator(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        *token = Token(TokenType::BadCharacter, GetLocation());
                        int32 advance;
                        PeekUtf32_FromUtf8(ptr, &advance);
                        ptr += advance;
                        token = outputs->Reserve();
                        hasBadCharacters = true;

                        goto loop;
                    }

                    case TokenizerState::StringInterpolation: {

                        if (*ptr == '{') {
                            leftBraceCount++;
                            *token = Token(TokenType::CurlyBraceOpen, GetLocation());
                            token = outputs->Reserve();
                            ptr++;
                            goto loop;
                        }

                        if (*ptr == '}') {
                            if (leftBraceCount == 0) {
                                PopState();
                                *token = Token(TokenType::LongStringInterpolationEnd, GetLocation());
                                token = outputs->Reserve();
                                ptr++;
                            }
                            else {

                                leftBraceCount--;
                                *token = Token(TokenType::CurlyBraceClose, GetLocation());
                                token = outputs->Reserve();
                                ptr++;
                            }
                            goto loop;
                        }

                        bool whitespaceComment = true;

                        while (whitespaceComment) {

                            whitespaceComment = false;

                            if (TryReadComment(token)) {
                                token = outputs->Reserve();
                                whitespaceComment = true;
                            }

                            if (TryReadWhitespace(token)) {
                                token = outputs->Reserve();
                                whitespaceComment = true;
                            }

                        }

                        if (!HasMoreInput()) {
                            break;
                        }

                        if (*ptr == '"') {

                            if (PeekStr(R"(""")", 3)) {
                                *token = Token(TokenType::MultiLineStringStart, GetLocation());
                                token = outputs->Reserve();
                                ptr += 3;
                                PushState(TokenizerState::MultiLineString);
                                goto loop;
                            }
                            else {
                                *token = Token(TokenType::StringStart, GetLocation());
                                token = outputs->Reserve();
                                ptr++;
                                PushState(TokenizerState::String);
                                goto loop;
                            }
                        }

                        if (*ptr == '`') {
                            *token = Token(TokenType::OpenStyle, GetLocation());
                            token = outputs->Reserve();
                            ptr++;
                            PushState(TokenizerState::Style);
                            goto loop;
                        }

                        if (*ptr == '\'') {
                            *token = Token(TokenType::OpenCharacter, GetLocation());
                            token = outputs->Reserve();
                            ptr++;
                            PushState(TokenizerState::Character);
                            goto loop;
                        }

                        if (TryReadIdentifierOrKeyword(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        if (TryReadNumber(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        if (TryMatchOperator(token)) {
                            token = outputs->Reserve();
                            goto loop;
                        }

                        *token = Token(TokenType::BadCharacter, GetLocation());

                        int32 advance;
                        PeekUtf32_FromUtf8(ptr, &advance);
                        ptr += advance;
                        token = outputs->Reserve();
                        hasBadCharacters = true;

                        goto loop;
                    }


                    case TokenizerState::Character: {
                        // we just eat everything and let the parser validate it
                        int32 stateStart = GetLocation();
                        char_state:
                        if (*ptr == '\'') {
                            PopState();
                            *token = Token(TokenType::RegularCharacterPart, stateStart);

                            token = outputs->Reserve();

                            *token = Token(TokenType::CloseCharacter, GetLocation());

                            token = outputs->Reserve();

                            ptr++;
                            goto loop;
                        }

                        int32 advance;
                        char32 c = GetCharUnchecked(&advance);

                        ptr += advance;

                        if (Unicode::IsNewLine(c)) {
                            *token = Token(TokenType::Whitespace, GetLocation() - advance); // maybe mark as unterminated instead?
                            token = outputs->Reserve();
                            goto loop;
                        }

                        goto char_state;
                    }

                    case TokenizerState::Style: {
                        int32 stateStart = GetLocation();
                        style_state:
                        if (*ptr == '`') {
                            PopState();
                            *token = Token(TokenType::RegularStringPart, stateStart);
                            token = outputs->Reserve();

                            *token = Token(TokenType::CloseStyle, GetLocation());
                            token = outputs->Reserve();

                            ptr++;
                            goto loop;
                        }

                        int32 advance;
                        char32 c = GetCharUnchecked(&advance);

                        ptr += advance;

                        if (Unicode::IsNewLine(c)) {
                            *token = Token(TokenType::Whitespace, GetLocation() - advance); // maybe mark as unterminated instead?
                            token = outputs->Reserve();
                            goto loop;
                        }

                        goto style_state;

                    }

                    case TokenizerState::String: {
                        int32 stringStart = outputs->size - 1;
                        int32 stateStart = GetLocation();
                        string_state:

                        char32 peek;
                        int32 a;
                        TryPeek32(&peek, &a);

                        if(Unicode::IsNewLine(peek)) {

                            PopState();
                            *token = Token(TokenType::RegularStringPart, stateStart);
                            outputs->GetPointer(stringStart)->flags |= TokenFlags::InvalidMatch;
                            token = outputs->Reserve();

                            ptr += a;
                            goto loop;

                        }

                        if (*ptr == '"') {
                            PopState();
                            *token = Token(TokenType::RegularStringPart, stateStart);
                            token = outputs->Reserve();

                            *token = Token(TokenType::StringEnd, GetLocation());
                            token = outputs->Reserve();

                            ptr++;
                            goto loop;
                        }

                        if (*ptr == '$') {
                            // if next is an identifier push a short template state
                            // -1 is safe because we're in the string state so we had at least a " in the buffer
                            if (ptr[-1] != '\\') {

                                ptr++;

                                if (*ptr == '{') {
                                    ptr++;
                                    // long template
                                    PushState(TokenizerState::StringInterpolation);
                                    *token = Token(TokenType::RegularStringPart, stateStart);
                                    token = outputs->Reserve();
                                    *token = Token(TokenType::LongStringInterpolationStart, GetLocation() - 2); // scoot back to encompass the ${

                                    token = outputs->Reserve();

                                    goto loop;
                                }

                                Token identifier;
                                if (TryReadIdentifierOrKeyword(&identifier)) {
                                    *token = Token(TokenType::RegularStringPart, stateStart);
                                    token = outputs->Reserve();

                                    stateStart = GetLocation();
                                    *token = Token(TokenType::ShortStringInterpolation, identifier.keyword, identifier.position - 1); // scoot back to encompass the $

                                    token = outputs->Reserve();
                                }

                                goto string_state; // didn't end up being an interpolate expression, just a lonely $
                            }

                        }

                        int32 advance;
                        char32 c = GetCharUnchecked(&advance);

                        ptr += advance;

                        if (Unicode::IsNewLine(c)) {
                            *token = Token(TokenType::Whitespace, GetLocation() - advance);
                            token = outputs->Reserve();
                            goto loop;
                        }

                        goto string_state;

                    }

                    case TokenizerState::MultiLineString: {
                        // we just eat everything and let the parser validate it
                        int32 stateStart = GetLocation();
                        multiline_string_state:
                        if (*ptr == '\"' && PeekStr(R"(""")", 3)) {

                            PopState();

                            *token = Token(TokenType::RegularStringPart, stateStart);

                            token = outputs->Reserve();

                            *token = Token(TokenType::MultiLineStringEnd, GetLocation());

                            token = outputs->Reserve();

                            ptr++;
                            goto loop;

                        }

                        // if next is an identifier push a short template state
                        // -1 is safe because we're in the string state so we had at least a " in the buffer
                        if (*ptr == '$' && ptr[-1] != '\\') {

                            int32 loc = GetLocation();
                            ptr++;

                            if (*ptr == '{') {
                                ptr++;
                                // long template
                                PushState(TokenizerState::StringInterpolation);

                                *token = Token(TokenType::RegularStringPart, stateStart);

                                token = outputs->Reserve();

                                *token = Token(TokenType::LongStringInterpolationStart, GetLocation() - 2);  // scoot back to encompass the ${

                                token = outputs->Reserve();

                                goto loop;
                            }

                            Token identifier;
                            if (TryReadIdentifierOrKeyword(&identifier)) {
                                *token = Token(TokenType::RegularStringPart, stateStart);
                                token = outputs->Reserve();

                                stateStart = GetLocation();

                                *token = Token(TokenType::ShortStringInterpolation, identifier.keyword, identifier.position - 1);  // scoot back to encompass the $
                                token = outputs->Reserve();
                            }

                            goto multiline_string_state; // didn't end up being an interpolate expression, just a lonely $
                        }

                        int32 advance;
                        char32 c = GetCharUnchecked(&advance);

                        ptr += advance;

                        if (Unicode::IsNewLine(c)) {
                            *token = Token(TokenType::Whitespace, GetLocation() - advance);

                            token = outputs->Reserve();
                            goto loop;
                        }

                        goto multiline_string_state;

                    }
                }

            }

            *token = Token(TokenType::EndOfInput, length);

            return !hasBadCharacters;

        }

    };
}