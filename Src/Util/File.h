#pragma once

#include "FixedCharSpan.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy {

    FixedCharSpan ReadFile(FixedCharSpan filePath, Allocator allocator);

    char* ReadFileIntoCString(const char* filename, int32* length);

}