#include <cstring>
#include "./Diagnostics.h"

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

    Diagnostic::Diagnostic(ErrorCode errorCode, FixedCharSpan span, FixedCharSpan message)
        : errorCode(errorCode)
        , start(span.ptr)
        , end(span.ptr + span.size)
        , messageLength(message.size)
        , message(message.ptr) {}

    Diagnostics::Diagnostics(Alchemy::Allocator allocator)
        : allocator(allocator)
        , capacity(16)
        , size(0)
        , array(allocator.AllocateUncleared<Diagnostic*>(capacity)) {}

    void Diagnostics::AddError(Diagnostic error) {
        if (size + 1 > capacity) {
            Diagnostic** newList = allocator.AllocateUncleared<Diagnostic*>(capacity * 2);
            memcpy(newList, array, sizeof(Diagnostic*) * size);
            allocator.Free(array, capacity);
            array = newList;
            capacity *= 2;
        }
        Diagnostic* ptr = allocator.AllocateUncleared<Diagnostic>(1);
        memcpy(ptr, &error, sizeof(Diagnostic));
        array[size++] = ptr;
    }

}