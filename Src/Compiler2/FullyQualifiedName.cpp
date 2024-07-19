#include "./FullyQualifiedName.h"
#include "../Util/StringUtil.h"

namespace Alchemy::Compilation {

    FixedCharSpan MakeFullyQualifiedGenericArgName(FixedCharSpan declaringTypeName, FixedCharSpan argumentName, int32 argIdx, Allocator allocator) {
        // System::Collections::Dictionary$2_Name[argIndex]
        int32 numberCnt = IntToAsciiCount(argIdx);
        size_t size = declaringTypeName.size + 3 + argumentName.size + numberCnt + 1;
        char* c = allocator.AllocateUncleared<char>(size);
        char* buffer = c;
        memcpy(c, declaringTypeName.ptr, declaringTypeName.size);
        c += declaringTypeName.size;
        c[0] = '_';
        c++;
        memcpy(c, argumentName.ptr, argumentName.size);
        c += argumentName.size;
        c[0] = '[';
        c++;
        c += IntToAscii(argIdx, c);
        c[0] = ']';
        c++;
        c[0] = '\0';
        return FixedCharSpan(buffer, (c - buffer));
    }

    FixedCharSpan MakeFullyQualifiedName(FixedCharSpan namespaceName, FixedCharSpan name, int32 genericCount, char* buffer) {

        char* ptr = buffer;

        memcpy(ptr, namespaceName.ptr, namespaceName.size);
        ptr += namespaceName.size;
        ptr[0] = ':';
        ptr[1] = ':';
        ptr += 2;
        memcpy(ptr, name.ptr, name.size);
        ptr += name.size;

        if (genericCount > 0) {
            ptr[0] = '$';
            ptr++;
            ptr += IntToAscii(genericCount, ptr);
        }

        *ptr = '\0';

        return FixedCharSpan(buffer, ptr - buffer);
    }

    FixedCharSpan MakeFullyQualifiedName(FixedCharSpan namespaceName, FixedCharSpan name, int32 genericCount, Allocator allocator) {

        size_t size = namespaceName.size + name.size + 2;

        if (genericCount > 0) {
            size += 1 + IntToAsciiCount(genericCount);
        }

        char* buffer = allocator.Allocate<char>(size);
        char* ptr = buffer;

        memcpy(ptr, namespaceName.ptr, namespaceName.size);
        ptr += namespaceName.size;
        ptr[0] = ':';
        ptr[1] = ':';
        ptr += 2;
        memcpy(ptr, name.ptr, name.size);
        ptr += name.size;

        if (genericCount > 0) {
            ptr[0] = '$';
            ptr++;
            IntToAscii(genericCount, ptr);
        }

        return FixedCharSpan(buffer, size);

    }


}