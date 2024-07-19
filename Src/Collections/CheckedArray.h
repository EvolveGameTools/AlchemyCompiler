#pragma once

#include "../PrimitiveTypes.h"
#include <type_traits>
#include <cstring>
#include "../Allocation/PodAllocation.h"


namespace Alchemy {

    template<class T>
    struct CheckedArray {
        T* array;
        int32 size;

        static_assert(std::is_trivially_copyable<T>());

        CheckedArray() : array(nullptr), size(0) {}

        CheckedArray(T* array, int32 size) : array(array), size(size) {}

        void Set(int32 index, const T& value) {
            assert(index >= 0 && index < size && "out of bounds");
            array[index] = value;
        }

        T& Get(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        T GetValue(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        T* GetPointer(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return &array[index];
        }

        T& operator[](int32 index) {
            RETURN_ARRAY_ACCESS(array, index, size);
        }

        void CopyFrom(CheckedArray<T> other) {
            memcpy(array, other.array, sizeof(T) * size);
        }

        CheckedArray<T> SliceStartEnd(int32 start, int32 end) {
            return CheckedArray<T>(array + start, end - start);
        }

        CheckedArray<T> SliceCount(int32 start, int32 count) {
            return CheckedArray<T>(array + start, count);
        }

        void Clear() {
            memset(array, 0, sizeof(T) * size);
        }

    };

}