#pragma once
#include "./LinearAllocator.h"

namespace Alchemy {

    TempAllocator* GetThreadLocalAllocator();

    void DisposeThreadLocalAllocator();

    #define TEMP_ALLOC_SCOPE_MARKER TempAllocator::ScopedMarker xxxxxxmarkerxxxxx(GetThreadLocalAllocator());

}