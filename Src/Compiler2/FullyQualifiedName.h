#pragma once

#include "../Util/FixedCharSpan.h"
#include "../Allocation/LinearAllocator.h"

namespace Alchemy::Compilation {

    FixedCharSpan MakeFullyQualifiedName(FixedCharSpan namespaceName, FixedCharSpan name, int32 genericCount, Allocator allocator);

    FixedCharSpan MakeFullyQualifiedName(FixedCharSpan namespaceName, FixedCharSpan name, int32 genericCount, char * buffer);

    FixedCharSpan MakeFullyQualifiedGenericArgName(FixedCharSpan declaringTypeName, FixedCharSpan argumentName, int32 argIdx, Allocator allocator);

}