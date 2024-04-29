#pragma once

#include <cstring>
#include <type_traits>
#include "CheckedArray.h"
#include "../Allocation/PodAllocation.h"

namespace Alchemy {
    template<class T>
    struct PodList {

        static_assert(std::is_trivially_copyable<T>());

        T* array;
        int32 size;
        int32 capacity;

        PodList() : array(nullptr), size(0), capacity(0) {}

        explicit PodList(int32 initialCapacity) : array(nullptr), size(0), capacity(0) {
            EnsureCapacity(initialCapacity);
        }

    public:

        int32 GetCapacity() const {
            return capacity;
        }

        void Add(T item) {
            if (size + 1 >= capacity) {
                EnsureCapacity(capacity == 0 ? 8 : capacity * 1.5);
            }
            array[size++] = item;
        }

        void Insert(T item, int32 index) {
            if (size + 1 >= capacity) {
                EnsureCapacity(capacity == 0 ? 8 : capacity * 1.5);
            }

            if(index < 0) {
                 index = 0;
            }

            if(index >= size) {
                index = size;
            }

            // Shift elements to the right to make space for the new element
            size_t bytesToMove = (size - index) * sizeof(T);
            memmove(&array[index + 1], &array[index], bytesToMove);

            // Insert the new element at the specified index
            array[index] = item;
            ++size;
        }

        T* Reserve() {
            if (size >= capacity) {
                EnsureCapacity(capacity == 0 ? 8 : capacity * 1.5);
            }
            T* retn = &array[size];
            size++;
            return retn;
        }

        T* Reserve(int32 cnt) {
            if (size + cnt >= capacity) {
                int32 a = size + (cnt * 2);
                int32 b = (int32)(capacity * 2);
                EnsureCapacity(capacity == 0 ? 8 : a  > b ? a : b);
            }
            T* retn = &array[size];
            size += cnt;
            return retn;
        }

        void AddRange(T* item, int32 count) {
            EnsureCapacity(size + count);
            std::memcpy(array + size, item, sizeof(T) * count);
            size += count;
        }

        void AddRange(const T* item, int32 count) {
            EnsureCapacity(size + count);
            std::memcpy(array + size, item, sizeof(T) * count);
            size += count;
        }

        void EnsureCapacity(int32 newCapacity) {
            if (capacity >= newCapacity) {
                return;
            }
            newCapacity = newCapacity < 8 ? 8 : newCapacity;

            T* newArray = MallocateTypedUncleared(T, newCapacity);
            if (array != nullptr) {
                std::memcpy(newArray, array, sizeof(T) * size);
                Mfree(array, capacity);
            }
            array = newArray;
            capacity = newCapacity;
        }

        void EnsureAdditionalCapacity(int32 additional) {
            EnsureCapacity(size + additional);
        }

        T& operator[](int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        void Push(const T & value) {
            Add(value);
        }

        T Pop() {
            assert(size > 0);
            T retn = array[--size];
            return retn;
        }

        T Peek() {
            assert(size > 0);
            T retn = array[size - 1];
            return retn;
        }

        void SwapRemoveAt(int32 index) {
            array[index] = array[--size];
        }

        void Clear() {
            size = 0;
        }

        void RemoveAt(int32 index) {
            if (index < 0 || index >= size) {
                return;
            }

            if (index == size - 1) {
                // If the element to be removed is the last one,
                // simply clear it and decrement the size
                memset(&array[size - 1], 0, sizeof(T));
                size--;
                return;
            }

            memmove(&array[index], &array[index + 1], (size - index) * sizeof(T));

            // Clear the last element
            memset(&array[size - 1], 0, sizeof(T));
            size--;

        }

        ~PodList() {
            Dispose();
        }

        T& Get(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        T* GetRefUnchecked(int32 index) {
            assert(index >= 0 && index < capacity && "out of bounds");
            return &array[index];
        }

        T* GetPointer(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return &array[index];
        }

        CheckedArray<T> ToCheckedArray() {
            return CheckedArray(array, size);
        }

        void Set(int32 index, const T& item) {
            assert(index >= 0 && index < size && "out of bounds");
            array[index] = item;
        }

        void Dispose() {
            if (array != nullptr) {
                Mfree(array, capacity * sizeof(T));
                array = nullptr;
            }
        }

    };

}