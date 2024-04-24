#pragma once

#include "../PrimitiveTypes.h"

template<typename T>
struct FixedRingBuffer {

#define RING_BUFFER_IS_POWER_OF_TWO(buffer_size) ((buffer_size & (buffer_size - 1)) == 0)

private:
    T* buffer;
    size_t mask;
    size_t tailIndex;
    size_t headIndex;
    int32 size;
public:
    FixedRingBuffer(T* buffer, size_t bufferSize)
        : buffer(buffer)
        , mask(bufferSize - 1)
        , size(0)
        , tailIndex(0)
        , headIndex(0) {
        assert(RING_BUFFER_IS_POWER_OF_TWO(bufferSize) == 1);
    }

    bool IsFull() const {
        return size == mask + 1;
    }

    int32 Size() const {
        return size;
    }

    void Enqueue(T item) {
        assert(!IsFull());

        size++;

        buffer[headIndex] = item;
        headIndex = (headIndex + 1) & mask;
    }

    bool TryDeque(T* item) {
        if (size == 0) {
            return false;
        }
        size--;
        *item = buffer[tailIndex];
        tailIndex = (tailIndex + 1) & mask;
        return true;
    }

};