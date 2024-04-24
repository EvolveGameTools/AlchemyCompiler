#pragma once
#include <type_traits>

namespace Alchemy {

    template<typename T>
    class PodQueue {

        T* array;
        int32 head;
        int32 tail;
        int32 size;
        int32 capacity;

        static_assert(std::is_trivially_copyable<T>());

    public:

        explicit PodQueue() : array(nullptr), head(0), tail(0), size(0), capacity(0) {}

        ~PodQueue() {
            if(array != nullptr) {
                FreeByteArray(array, alignof(T));
            }
            array = nullptr;
            head = 0;
            tail = 0;
            size = 0;
            capacity = 0;
        }

        void Clear() {
            head = 0;
            tail = 0;
            size = 0;
        }

        int32 Size() const {
            return size;
        }

        void Enqueue(const T & item) {
            if (size >= capacity - 1) {
                SetCapacity();
            }

            array[tail] = item;
            tail = (tail + 1) & (capacity - 1);
            ++size;
        }

        bool TryDequeue(T * retn) {
            if (size == 0) return false;
            *retn = array[head];
            head = (head + 1) & (capacity - 1);
            --size;
            return true;
        }

        void SetCapacity() {
            int32 newCapacity = capacity * 2;
            if (newCapacity < 16) newCapacity = 16;
            newCapacity = MathUtil::CeilPow2(newCapacity);
            T * destinationArray = (T*)MallocByteArray(newCapacity * sizeof(T), alignof(T));

            int j = 0;
            int i = head;

            if (size > 0) {
                while (i != tail) {
                    destinationArray[j++] = array[i];
                    i = (i + 1) & (capacity - 1); // Equivalent to: i = (i + 1) % capacity
                }

            }

            FreeByteArray(array, alignof(T));

            array = destinationArray;
            head = 0;
            tail = j;
            capacity = newCapacity;
        }

    };

}