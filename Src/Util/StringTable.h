#pragma once

#include "../PrimitiveTypes.h"
#include "./FixedCharSpan.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy {

    class StringTable {

        int32 size;
        int32 exponent;
        FixedCharSpan* table;
        Allocator allocator;

    public:

        StringTable(Allocator allocator, int32 initialCapacity);

        ~StringTable();

        FixedCharSpan Intern(FixedCharSpan toIntern);

    };

}