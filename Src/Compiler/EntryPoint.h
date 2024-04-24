#pragma once

namespace Alchemy::Compilation {

    struct EntryPoint {

        MethodInfo* methodInfo;

        explicit EntryPoint(MethodInfo* methodInfo)
            : methodInfo(methodInfo) {}

        bool Equals(EntryPoint& other) const {
            return methodInfo == other.methodInfo;
        }

        int32 GetHashCode() const {
            uintptr_t ptrValue = reinterpret_cast<uintptr_t>(methodInfo);
            uint32_t hash = static_cast<uint32_t>(ptrValue);
            hash ^= (ptrValue >> 32);
            return (int32)hash;
        }

    };


}