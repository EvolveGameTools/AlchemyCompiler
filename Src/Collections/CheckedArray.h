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

        // static_assert(std::is_trivially_copyable<T>());

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
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
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

    template<class T>
    struct ResizableArray {

        T* array {};
        int32 size {};

        static_assert(std::is_trivially_copyable<T>());

        ResizableArray() : array(nullptr), size(0) {}

        explicit ResizableArray(int32 size)
            : array((T*) MallocByteArray(size * sizeof(T), alignof(T)))
            , size(size) {}

        ~ResizableArray() {
            if (array != nullptr) {
                FreeByteArray(array, alignof(T));
            }
            array = nullptr;
            size = 0;
        }

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
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        CheckedArray<T> ToCheckedArray() {
            return CheckedArray<T>(array, size);
        }

        void Clear() {
            memset(array, 0, sizeof(T) * size);
        }

        void EnsureCapacity(int32 requirement) {
            if (size >= requirement) return;
            T* newArray = (T*) MallocByteArray(requirement * sizeof(T), alignof(T));
            if (array != nullptr) {
                memcpy(newArray, array, sizeof(T) * size);
                FreeByteArray(array, alignof(T));
            }
            int32 itemCount = requirement - size;
            memset(newArray + size, 0, sizeof(T) * itemCount);
            array = newArray;
            size = requirement;
        }

    };

}