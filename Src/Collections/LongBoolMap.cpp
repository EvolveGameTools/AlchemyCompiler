#include "LongBoolMap.h"
#include <cstring>

#if _MSC_VER
#include <intrin.h>
#  define __builtin_popcountll __popcnt64
#endif

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

    void LongBoolMap::Combine(LongBoolMap other) {
        for (int32 i = 0; i < size; i++) {
            map[i] |= other.map[i];
        }
    }

    bool LongBoolMap::ContainsAny(LongBoolMap other) {
        int32 min = size < other.size ? size : other.size;

        for (int32 i = 0; i < min; i++) {
            if((map[i] & other.map[i]) != 0) {
                return true;
            }
        }

        return false;
    }

//    leaving this here for posterity
//    #include <atomic>
//    void LongBoolMap::AtomicOr(LongBoolMap a, LongBoolMap b) {
//
//        // we want to atomically update our map, convert our map memory to atomics.
//        // this only works if we allocated the map with an 8 byte alignment (but we always so)
//        std::atomic<uint64_t> * atomicValues = reinterpret_cast<std::atomic<uint64_t>*>(a.map);
//
//        for (int32 i = 0; i < a.size; i++) {
//            uint64_t expected = atomicValues[i].load(); // get the current value of the map at [i]
//            uint64_t newValue = expected | b.map[i]; // compute the new value of the map at [i]
//
//            // keep trying to atomically update to new_value until successful
//            while (!atomicValues[i].compare_exchange_weak(expected, newValue)) {
//                expected = atomicValues[i].load(); // if update fails, get the current value of a[i] again
//                newValue = expected | b.map[i]; // recompute the new value
//            }
//        }
//
//    }

}