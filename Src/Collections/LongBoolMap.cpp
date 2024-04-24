#include "LongBoolMap.h"
#include <cstring>

namespace Alchemy {

    bool LongBoolMap::Get(int32 idx) {
        int32 mapIdx = idx >> 6; // divide by 64
        int32 shift = (idx - (mapIdx << 6)); // multiply by 64
        return (map[mapIdx] & ((1ull << shift))) != 0;
    }

    void LongBoolMap::Set(int idx) {
        int32 mapIdx = idx >> 6;
        int32 shift = (idx - (mapIdx << 6));
        map[mapIdx] |= (1ull << shift);
    }

    void LongBoolMap::Unset(int32 idx) {
        int32 mapIdx = idx >> 6;
        int32 shift = (idx - (mapIdx << 6));
        map[mapIdx] &= ~(1ull << shift);
    }

    bool LongBoolMap::TrySetIndex(int32 index) {
        int32 mapIdx = index >> 6; // divide by 64
        int32 shift = (index - (mapIdx << 6)); // multiply by 64

        uint64 value = map[mapIdx];
        map[mapIdx] |= 1ull << shift;
        return value != map[mapIdx];
    }

    bool LongBoolMap::TryUnsetIndex(int32 index) {
        int32 mapIdx = index >> 6; // divide by 64
        int32 shift = (index - (mapIdx << 6)); // multiply by 64

        uint64 value = map[mapIdx];
        map[mapIdx] &= ~(1ull << shift);
        return value != map[mapIdx];
    }

    void LongBoolMap::Clear() {
        memset(map, 0, sizeof(uint64) * size);
    }

    int32 LongBoolMap::PopCount() {
        int32 retn = 0;
        for (int i = 0; i < size; i++) {
            if(map[i] != 0) {
                retn += __builtin_popcountll(map[i]);
            }
        }

        return retn;
    }

    bool LongBoolMap::TrySetIndex(uint64* map, int32 index) {
        int32 mapIdx = index >> 6; // divide by 64
        int32 shift = index - (mapIdx << 6); // multiply by 64
        uint64 value = map[mapIdx];
        map[mapIdx] |= 1ull << shift;
        return value != map[mapIdx];
    }

    bool LongBoolMap::TryUnsetIndex(uint64* map, int32 index) {
        int32 mapIdx = index >> 6; // divide by 64
        int32 shift = index - (mapIdx << 6); // multiply by 64
        uint64 value = map[mapIdx];
        map[mapIdx] &= ~(1ull << shift);
        return value != map[mapIdx];
    }

    bool LongBoolMap::IsMapBitSet(const uint64* ptr, int idx) {
        int32 mapIdx = idx >> 6; // divide by 64
        int32 shift = idx - (mapIdx << 6); // multiply by 64
        return (ptr[mapIdx] & (1ull << shift)) != 0;
    }

    LongBoolMapEnumerator LongBoolMap::GetEnumerator() {
        return LongBoolMapEnumerator(map, size);
    }

    void LongBoolMap::SetMapBit(uint64* map, int32 index) {
        int32 mapIdx = index >> 6;
        int32 shift = (index - (mapIdx << 6));
        map[mapIdx] |= (1ull << shift);
    }

    void LongBoolMap::Flatten(int32* retn) {
        int32 idx = 0;
        for (int i = 0; i < size; i++) {
            int64 value = (int64)map[i];
            while (value != 0) {
                // x & -x returns an integer with only the lsb of the value set to 1
                int64 t = value & -value;
                int32 tzcnt = tzcnt64(value); // counts the number of trailing 0s to lsb
                value ^= t; // toggle the target bit off
                retn[idx++] = (i * 64) + tzcnt;
            }
        }
    }

    LongBoolMap::LongBoolMap(uint64* map, int32 size) : map(map), size(size) {}

    void LongBoolMap::ToBuffer(CheckedArray<int32> retn) {
        int32 idx = 0;
        for (int32 i = 0; i < size; i++) {
            uint64 value = (uint64)map[i];
            while (value != 0) {
                // x & -x returns an integer with only the lsb of the value set to 1
                uint64 t = value & -value;
                int32 tzcnt = tzcnt64((int64)value); // counts the number of trailing 0s to lsb
                value ^= t; // toggle the target bit off
                retn.Set(idx++,  ((i * 64) + tzcnt));
            }
        }
    }

    bool LongBoolMapEnumerator::MoveNext() {
        while (index < maxIndex) {
            if (currentPtrValue != 0) {
                int32 result = (index * 64) + tzcnt64(currentPtrValue);
                currentPtrValue ^= currentPtrValue & -currentPtrValue;
                current = result;
                return true;
            }

            index++;
            currentPtrValue = ptr[index];
        }

        return false;
    }

    bool LongBoolMapEnumerator::MoveNext(int32* outValue) {
        while (index < maxIndex) {
            if (currentPtrValue != 0) {
                int32 x = (index * 64);
                int32 tzcnt = tzcnt64(currentPtrValue);
                int32 result = x + tzcnt; // (index * 64) + tzcnt64(currentPtrValue);
                currentPtrValue ^= currentPtrValue & -currentPtrValue;
                *outValue = result;
                return true;
            }

            index++;
            currentPtrValue = ptr[index];
        }

        return false;
    }

    int LongBoolMapEnumerator::Current() const {
        return current;
    }

    LongBoolMapEnumerator::LongBoolMapEnumerator(uint64* map, int32 size) {
        index = 0;
        if (size == 0) {
            ptr = nullptr;
            currentPtrValue = 0;
        }
        else {
            ptr = (int64*) map;
            currentPtrValue = ptr[0];
        }

        maxIndex = size;
        current = 0;
    }
}