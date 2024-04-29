#pragma once

#include "../PrimitiveTypes.h"
#include <type_traits>
#include <cstring>
#include "../Allocation/PodAllocation.h"
#include "./CheckedArray.h"

namespace Alchemy {

    // Doesn't delete itself, has no destructor, requires an explicit resize if desired
    template<class T>
    struct AllocatedArray {

        T* array {};
        int32 size {};

        static_assert(std::is_trivially_copyable<T>());

        AllocatedArray() : array(nullptr), size(0) {}

        AllocatedArray(T * array, int32 size) : array(array), size(size) {}

        void Set(int32 index, const T& value) {
            assert(index >= 0 && index < size && "out of bounds");
            array[index] = value;
        }

        T& Get(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        T* GetPointer(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return &array[index];
        }

        T& operator[](int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        CheckedArray<T> ToCheckedArray() {
            return CheckedArray<T>(array, size);
        }

        void Clear() {
            memset(array, 0, sizeof(T) * size);
        }

    };
}