#pragma once

#include "../PrimitiveTypes.h"
#include "CheckedArray.h"

namespace Alchemy {

    struct LongBoolMapEnumerator {

        int64* ptr;
        int32 index;
        int32 maxIndex;
        int64 currentPtrValue;
        int32 current;

        LongBoolMapEnumerator(uint64* map, int32 size) {
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

        int Current() const {
            return current;
        }

        bool MoveNext() {
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

        bool MoveNext(int32 * outValue) {
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

    };

    struct LongBoolMap {

        uint64 * map;
        int32 size;

        LongBoolMap() : map(nullptr), size(0) {}
        LongBoolMap(uint64 *map, int32 size);
        ~LongBoolMap() = default;

        bool Get(int32 idx);
        void Set(int32 idx);
        void Unset(int32 idx);
        bool TrySetIndex(int32 idx);
        bool TryUnsetIndex(int32 idx);
        void Flatten(int32 * list);

        int32 PopCount();
        void Clear();

        LongBoolMapEnumerator GetEnumerator();

        static bool TrySetIndex(uint64* map, int32 index);
        static bool TryUnsetIndex(uint64* map, int32 index);
        static void SetMapBit(uint64* map, int32 index);

        static int32 GetMapSize(int32 itemCount) {
            // round to nearest 64, divide by 64
            return  ((itemCount + 63) & -64) / 64;
        }

        static bool IsMapBitSet(const uint64* ptr, int idx);

        void ToBuffer(CheckedArray<int32> array);

        void Combine(LongBoolMap map);

        bool ContainsAny(LongBoolMap map);
    };

}