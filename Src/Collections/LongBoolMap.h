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

        LongBoolMapEnumerator(uint64* map, int32 size);

        int32 Current() const;

        bool MoveNext();

        bool MoveNext(int32 * outValue);

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