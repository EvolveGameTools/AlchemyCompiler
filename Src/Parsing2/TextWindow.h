#pragma once
#include "../PrimitiveTypes.h"

namespace Alchemy::Parsing {

    struct TextWindow {

        char* ptr;
        char* start;
        char* end;

        TextWindow(char* string, size_t length);

        bool Peek(char c);
        char PeekChar();
        char PeekChar(int32 steps);
        char32 PeekChar32(int32 * advance);
        bool TryPeekChar32(char32* c, int32* advance);
        void Advance(int32 steps = 1);

        bool HasMoreContent();

        bool TryAdvance(char i);
    };

}