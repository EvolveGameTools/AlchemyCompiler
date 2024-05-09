#include <cstring>
#include "./Diagnostics.h"
#include "../Allocation/BytePoolAllocator.h"

namespace Alchemy::Compilation {

    Diagnostic::Diagnostic(ErrorCode errorCode, char* start, char* end)
        : errorCode(errorCode)
        , start(start)
        , end(end)
        , messageLength(0)
        , message(nullptr) {}

    Diagnostic::Diagnostic(ErrorCode errorCode, Alchemy::FixedCharSpan span)
        : errorCode(errorCode)
        , start(span.ptr)
        , end(span.ptr + span.size)
        , messageLength(0)
        , message(nullptr) {}

    Diagnostics::Diagnostics(Alchemy::LinearAllocator* allocator)
        : allocator(allocator)
        , capacity(128)
        , size(0)
        , array(allocator->AllocateUncleared<Diagnostic*>(capacity)) {}


    void Diagnostics::AddError(Diagnostic error) {
        if (size + 1 > capacity) {
            Diagnostic** newList = allocator->AllocateUncleared<Diagnostic*>(capacity * 2);
            memcpy(newList, array, sizeof(Diagnostic*) * size);
            array = newList;
            capacity *= 2;
        }
        Diagnostic* ptr = allocator->AllocateUncleared<Diagnostic>(1);
        memcpy(ptr, &error, sizeof(Diagnostic));
        array[size++] = ptr;
    }

}