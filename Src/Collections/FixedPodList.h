#pragma once

#include "CheckedArray.h"

namespace Alchemy {

    template<class T>
    struct FixedPodList {

        T* array;
        int32 size;
        int32 capacity;

        FixedPodList() : array(nullptr), size(0), capacity(0) {}

        FixedPodList(T* array, int32 capacity) : array(array), size(0), capacity(capacity) {}

        bool Add(const T& value) {

            if (size >= capacity) {
                return false;
            }

            array[size++] = value;
            return true;
        }

        T& AddRef() {
            assert(size < capacity);
            T& retn = array[size];
            size++;
            return retn;
        }

        void SwapRemoveAt(int32 idx) {
            assert(idx >= 0 && idx < size);
            array[idx] = array[--size];
        }

        void SetSize(int32 newSize) {
            assert(newSize >= 0 && newSize <= capacity && "out of bounds");
            size = newSize;
        }

        void EnsureSize(int32 newSize) {
            if (size >= newSize) return;
            assert(newSize >= 0 && newSize <= capacity && "out of bounds");
            memset(array + size, 0, (newSize - size) * sizeof(T));
            size = newSize;
        }

        void Set(int32 index, const T& value) {
            assert(index >= 0 && index < size && "out of bounds");
            array[index] = value;
        }

        T& Get(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        T* GetRef(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return &array[index];
        }

        void Clear() {
            memset(array, 0, size * sizeof(T));
            size = 0;
        }

        T& operator[](int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array[index];
        }

        CheckedArray<T> ToCheckedArray() {
            return CheckedArray<T>(array, size);
        }

        T* GetPointer(int32 index) {
            assert(index >= 0 && index < size && "out of bounds");
            return array + index;
        }

        T & Peek() {
            assert(size > 0);
            return array[size - 1];
        }

        void Push(T& item) {
            Add(item);
        }

        void Push(const T& item) {
            Add(item);
        }

        T Pop() {
            size--;
            return array[size];
        }

        T* Reserve(int32 count) {
            assert(size + count <= capacity && "out of bounds");
            T * retn = &array[size];
            size += count;
            return retn;
        }

        void AddRange(T * item, int32 count) {
            for(int32 i = 0; i < count; i++) {
                Add(item[i]);
            }
        }

    };

}