#pragma once

#include "../PrimitiveTypes.h"

namespace Alchemy {

    // output must be at least 13 chars in size
    void EncodePointerBase32(void* ptr, char* output);

    int32 IntToAsciiCount(int32 num);

    int32 IntToAscii(int32 num, char* str);

}