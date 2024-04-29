#pragma once
#include "./LinearAllocator.h"

namespace Alchemy {

    TempAllocator* GetThreadLocalAllocator();

    void DisposeThreadLocalAllocator();

}