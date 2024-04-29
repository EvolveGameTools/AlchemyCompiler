#include <cstdio>
#include "./TextWindow.h"

namespace Alchemy::Parsing {

    TextWindow::TextWindow(char* string, size_t length)
        : start(string)
        , ptr(string)
        , end(string + length) {}

    bool TextWindow::Peek(char c) {
        return ptr != end && *ptr == c;
    }

    char TextWindow::PeekChar(int32 steps) {
        return ptr + steps < end ? ptr[steps] : '\0';
    }

    char TextWindow::PeekChar() {
        return ptr == end ? '\0' : *ptr;
    }

    void TextWindow::Advance(int32 steps) {
        ptr += steps;
        if (ptr > end) {
            ptr = end;
        }
    }

    bool TextWindow::HasMoreContent() {
        return ptr < end;
    }

    bool TextWindow::TryPeekChar32(char32* c, int32* advance) {
        if (ptr >= end) {
            *advance = 0;
            return false;
        }

        char32 result;
        // Check the number of bytes in the current UTF-8 char based on the first byte
        if ((*ptr & 0x80) == 0) {           // 0xxxxxxx
            *c = *ptr;
            *advance = 1;
            return true;
        }

        if ((*ptr & 0xE0) == 0xC0) {  // 110xxxxx
            result = (*ptr & 0x1F) << 6;
            result |= (ptr[1] & 0x3F);
            *advance = 2;
            *c = result;
            return true;
        }

        if ((*ptr & 0xF0) == 0xE0) {  // 1110xxxx
            result = (*ptr & 0x0F) << 12;
            result |= (ptr[1] & 0x3F) << 6;
            result |= (ptr[2] & 0x3F);
            *advance = 3;
            *c = result;
            return true;
        }

        if ((*ptr & 0xF8) == 0xF0) {  // 11110xxx
            result = (*ptr & 0x07) << 18;
            result |= (ptr[1] & 0x3F) << 12;
            result |= (ptr[2] & 0x3F) << 6;
            result |= (ptr[3] & 0x3F);
            *advance = 4;
            *c = result;
            return true;
        }

        *advance = 0;
        return false;

    }

    char32 TextWindow::PeekChar32(int32* advance) {

        if (ptr < end) {
            *advance = 0;
            return 0;
        }

        char32 result = 0;
        // Check the number of bytes in the current UTF-8 char based on the first byte
        if ((*ptr & 0x80) == 0) {           // 0xxxxxxx
            result = *ptr;
            *advance = 1;
        }
        else if ((*ptr & 0xE0) == 0xC0) {  // 110xxxxx
            result = (*ptr & 0x1F) << 6;
            result |= (ptr[1] & 0x3F);
            *advance = 2;
        }
        else if ((*ptr & 0xF0) == 0xE0) {  // 1110xxxx
            result = (*ptr & 0x0F) << 12;
            result |= (ptr[1] & 0x3F) << 6;
            result |= (ptr[2] & 0x3F);
            *advance = 3;
        }
        else if ((*ptr & 0xF8) == 0xF0) {  // 11110xxx
            result = (*ptr & 0x07) << 18;
            result |= (ptr[1] & 0x3F) << 12;
            result |= (ptr[2] & 0x3F) << 6;
            result |= (ptr[3] & 0x3F);
            *advance = 4;
        }
        else {
            *advance = 0;
        }

        return result;
    }

    bool TextWindow::TryAdvance(char c) {
        if(Peek(c)) {
            Advance();
            return true;
        }
        return false;
    }


}