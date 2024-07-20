#pragma once

#include <cstring>
#include "../PrimitiveTypes.h"

namespace Alchemy {

    // output must be at least 13 chars in size
    void EncodePointerBase32(void* ptr, char* output);

    int32 IntToAsciiCount(int32 num);

    int32 IntToAscii(int32 num, char* str);

    #define PrintFlagVars(buffer) size_t printflag_len = 0; char * printflag_c = buffer
    #define PrintFlagLength printflag_len
    #define PrintFlag(x, y) if((flags & x::y) != 0)  { printflag_len += CountAndMaybeWrite(#y, &printflag_c, printflag_len); }
    #define PrintFlagLower(x, y) if((flags & x::y) != 0)  { printflag_len += CountAndMaybeWriteLower(#y, &printflag_c, printflag_len); }
    #define PrintFlagZero(x, y) if((flags == x::y) != 0)  if (flags == x::y) { PrintFlagLength += CountAndMaybeWrite(#y, &printflag_c, printflag_len); return PrintFlagLength;  }

    size_t CountAndMaybeWrite(const char* str, char** buffer, size_t offset);
    size_t CountAndMaybeWriteLower(const char* str, char** buffer, size_t offset);

}