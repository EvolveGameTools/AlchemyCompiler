//#pragma once
//
//#include <cstring>
//#include "LongBoolMap.h"
//#include "PodList.h"
//#include "../Allocation/PodAllocation.h"
//
//namespace Alchemy {
//
//    struct StringKey {
//
//        char* cbuffer;
//        int32 length;
//
//        StringKey(char* buffer, int32 length) : cbuffer(buffer), length(length) {}
//
//        int32 GetHashCode() const {
//
//            // FNV-1a
//
//            constexpr uint32 FNV_PRIME_32 = 16777619;
//            constexpr uint32 OFFSET_BASIS_32 = 2166136261;
//
//            uint32 hash = OFFSET_BASIS_32;
//
//            for (size_t i = 0; i < length; ++i) {
//                hash ^= static_cast<uint32>(cbuffer[i]);
//                hash *= FNV_PRIME_32;
//            }
//
//            return static_cast<int32>(hash);
//        }
//
//        bool Equals(StringKey other) const {
//            if (cbuffer == other.cbuffer) {
//                return true;
//            }
//
//            if (length != other.length) {
//                return false;
//            }
//
//            return memcmp(cbuffer, other.cbuffer, length) == 0;
//
//        }
//
//    };
//
//    struct IntKey {
//
//        int32 key;
//
//        explicit IntKey(int32 key) : key(key) {}
//
//        int32 GetHashCode() const {
//            int32 x = key;
//            // doing this twice in release mode somehow busts the hash in ways I don't understand
//            x = ((x >> 16) ^ x) * 0x45d9f3b;
//            x = ((x >> 16) ^ x) * 0x45d9f3b;
//            x = (x >> 16) ^ x;
//            return x;
//        }
//
//        bool Equals(IntKey other) const {
//            return key == other.key;
//        }
//
//    };
//
//    struct MSIMapUtil {
//
//        static uint64 Align(size_t number, size_t alignment) {
//            return ((number + (alignment - 1)) & (-alignment));
//        }
//
//        static constexpr float k_FillFactor = 0.5f;
//        static constexpr int k_StepSubtract = 32;
//
//        static constexpr size_t k_Alignment = 16;
//
//        static int32 InitBuffers(int32 exponent, uint64** pNewMap, uint64** pNewGravestones, uint8** pNewKeys, uint8** pNewValues, size_t keyTypeSize, size_t valueTypeSize) {
//            int32 shiftedExponent = 1 << exponent;
//
//            uint64 mapSize = (int64) Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
//            uint64 graveStoneSize = pNewGravestones != nullptr ? mapSize : 0;
//            uint64 keySize = (int64) Align(keyTypeSize * shiftedExponent, 8);
//            uint64 valueSize = (int64) Align(valueTypeSize * shiftedExponent, 8);
//
//            uint8* bytes = Allocate(mapSize + graveStoneSize + keySize + valueSize);
//
//            // need to clear keys in case we compare with a key value that randomly
//            // happens to be a valid value in memory but isn't a real key.
//            // we don't clear values since they only get read one success
//
//            memset(bytes, 0, mapSize + graveStoneSize + keySize);
//
//            *pNewMap = (uint64*) bytes;
//            *pNewKeys = (bytes + mapSize);
//
//            *pNewMap = (uint64*) bytes;
//            if (pNewGravestones != nullptr) {
//                *pNewGravestones = (uint64*) (bytes + mapSize);
//            }
//            *pNewKeys = (bytes + mapSize + graveStoneSize);
//            if (pNewValues != nullptr) {
//                *pNewValues = (bytes + mapSize + graveStoneSize + keySize);
//            }
//
//            return (int32) (shiftedExponent * k_FillFactor);
//        }
//
//        static uint8* Allocate(size_t size) {
//            return (uint8*) MallocByteArray(size, k_Alignment);
//        }
//
//
//        static void Clear(int32 exponent, void* allocation, size_t keyTypeSize, bool hasGravestones) {
//            int32 shiftedExponent = 1 << exponent;
//
//            uint64 mapSize = Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
//            uint64 gravestoneSize = hasGravestones ? mapSize : 0;
//            uint64 keySize = Align(keyTypeSize * shiftedExponent, 8);
//
//            // need to clear keys in case we compare with a key value that randomly
//            // happens to be a valid value in memory but isn't a real key.
//            // we don't clear values since they only get read one success
//            memset(allocation, 0, mapSize + gravestoneSize + keySize);
//
//        }
//
//        static void Free(void* memory) {
//            FreeByteArray(memory, 16);
//        }
//
//        typedef int32 (*getHashCode)(void*, uint8*);
//
//        static void Resize(int32* exponent, int32* threshold, uint64** pMap, uint64** pGraveStones, uint8** pKeys, uint8** pValues, size_t keyTypeSize, size_t valueTypeSize, void* cookie, getHashCode hashCodeFn) {
//
//            // Note: if we resize this we end up changing the order of the keys, which means we lose the guarantee
//            // that values are returned in the order that the keys are added! In order to actually support that use case
//            // we would need to keep an additional ordered list of keys. I've chosen not to do this. If the caller needs the values
//            // back in a certain order they should just sort the list themselves.
//
//            LongBoolMapEnumerator mapIterator(*pMap, LongBoolMap::GetMapSize(1 << *exponent));
//            *exponent = *exponent + 1; // bump exponent, capacity is 1 << exponent
//
//            uint64* pNewMap;
//            uint64* pNewGravestones;
//            uint8* pNewKeys;
//            uint8* pNewValues;
//
//            *threshold = MSIMapUtil::InitBuffers(
//                    *exponent,
//                    &pNewMap,
//                    pGraveStones == nullptr ? nullptr : &pNewGravestones,
//                    &pNewKeys,
//                    pValues == nullptr ? nullptr : &pNewValues,
//                    keyTypeSize,
//                    valueTypeSize
//            );
//
//            int32 exp = *exponent;
//            int32 mask = (1 << exp) - 1;
//
//            uint8* prevKeys = *pKeys;
//            uint8* prevValues = pValues == nullptr ? nullptr : *pValues;
//
//            while (mapIterator.MoveNext()) {
//                int32 prevIndex = mapIterator.Current();
//                int32 hashCode = hashCodeFn(cookie, &prevKeys[prevIndex]);
//                int32 step = hashCode >> (k_StepSubtract - exp) | 1;
//
//                int32 index = (hashCode + step) & mask;
//
//                while (!LongBoolMap::TrySetIndex(pNewMap, index)) {
//                    index = (index + step) & mask;
//                }
//
//                memcpy(&pNewKeys[index], &prevKeys[prevIndex], keyTypeSize);
//
//                if (pValues != nullptr) {
//                    memcpy(&pNewValues[index], &prevValues[prevIndex], valueTypeSize);
//                }
//
//            }
//
//            Free(*pMap);
//
//            *pMap = pNewMap;
//            *pKeys = pNewKeys;
//            if (pGraveStones != nullptr) {
//                *pGraveStones = pNewGravestones;
//            }
//            if (pValues != nullptr) {
//                *pValues = pNewValues;
//            }
//
//
//        }
//
//    };
//
//    template<typename TKey>
//    struct MSIIterator {
//
//        TKey key;
//        int32 index;
//        int32 mask;
//        int32 step;
//
//        explicit MSIIterator(TKey key, int32 index, int32 mask, int32 step) : key(key), index(index), mask(mask), step(step) {}
//
//    };
//
//    template<typename TKey, typename TValue>
//    struct MSIDictionary {
//        int32 exponent;
//        int32 threshold;
//        int32 size;
//        uint64* pMap;
//        TKey* pKeys;
//        TValue* pValues;
//
//        MSIDictionary() : MSIDictionary(16) {}
//
//        explicit MSIDictionary(int32 baseCapacity)
//        : exponent(0)
//        , threshold(0)
//        , size(0)
//        , pMap(nullptr)
//        , pKeys(nullptr)
//        , pValues(nullptr) {
//            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
//            threshold = MSIMapUtil::InitBuffers(exponent, &pMap, &pKeys, &pValues);
//        }
//
//        ~MSIDictionary() {
//            MSIMapUtil::Free(GetAllocation());
//        }
//
//        void Set(TKey key, TValue value) {
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                // add as a new entry
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    pKeys[index] = key;
//                    pValues[index] = value;
//                    if (size + 1 >= threshold) {
//                        MSIMapUtil::Resize(&exponent, &threshold, &pMap, &pKeys, &pValues);
//                    }
//                    size++;
//                    return;
//                }
//
//                // reuse the previous one and update the value
//                if (pKeys[index].Equals(key)) {
//                    pValues[index] = value;
//                    return;
//                }
//            }
//        }
//
//        bool Contains(TKey key) {
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    return false;
//                }
//
//                if (pKeys[index].Equals(key)) {
//                    return true;
//                }
//
//            }
//
//        }
//
//        int32 GetValues(CheckedArray<TValue> outValues) {
//            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
//            int32 keyIndex;
//            int32 idx = 0;
//            while (enumerator.MoveNext(&keyIndex)) {
//                outValues.Set(idx++, pValues[keyIndex]);
//            }
//            return idx;
//        }
//
//        bool TryGetValue(TKey key, TValue* value) {
//
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    return false;
//                }
//
//                if (pKeys[index].Equals(key)) {
//                    *value = pValues[index];
//                    return true;
//                }
//
//            }
//        }
//
//        bool TryAdd(TKey key, const TValue & value) {
//            if(Contains(key)) {
//                return false;
//            }
//
//            if (size + 1 >= threshold) {
//                MSIMapUtil::Resize(&exponent, &threshold, &pMap, &pKeys, &pValues);
//            }
//
//            int32 h = key.GetHashCode();
//
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = (h + step) & mask;
//
//            while (!LongBoolMap::TrySetIndex(pMap, index)) {
//                index = (index + step) & mask;
//            }
//
//            pKeys[index] = key;
//            pValues[index] = value;
//            size++;
//
//            return true;
//        }
//
//        uint8* GetAllocation() {
//            return (uint8*) pMap;
//        }
//
//        void Clear() {
//            int32 shiftedExponent = 1 << exponent;
//
//            // need to clear keys in case we compare with a key value that randomly
//            // happens to be a valid value in memory but isn't a real key.
//            // we don't clear values since they only get read one success
//            size = 0;
//
//            memset(pMap, 0, sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent));
//            memset(pKeys, 0, sizeof(TKey) * shiftedExponent);
//
//        }
//
//    };
//
//
//    template<typename TKey>
//    struct MSISet { // todo -- make a real set w/o value allocations-
//
//        MSIDictionary<TKey, uint8> dictionary;
//
//        explicit MSISet(int32 initialCapacity)
//                : dictionary(initialCapacity)
//        {}
//
//        void Initialize(int32 capacity) {
//            dictionary.Initialize(capacity);
//        }
//
//        bool TryAdd(TKey key){
//            return dictionary.TryAdd(key, 0);
//        }
//
//        bool Contains(TKey key) {
//            return dictionary.Contains(key);
//        }
//
//        int32 GetSize() const {
//            return dictionary.size;
//        }
//
//        void Clear() {
//            dictionary.Clear();
//        }
//
//        void GetKeys(CheckedArray<TKey> outKeys) {
//            LongBoolMapEnumerator enumerator(dictionary.pMap, LongBoolMap::GetMapSize(1 << dictionary.exponent));
//            int32 keyIndex;
//            int32 idx = 0;
//            while (enumerator.MoveNext(&keyIndex)) {
//                outKeys.Set(idx, dictionary.pKeys[keyIndex]);
//                idx++;
//            }
//        }
//
//    };
//
//    template<typename TKey, typename TValue>
//    struct MSIIndexDictionary {
//
//        int32 exponent;
//        int32 threshold;
//        int32 size;
//        PodList<TValue> values;
//        uint64* pMap;
//        TKey* pKeys;
//        int32* pValues;
//
//        MSIIndexDictionary() : MSIIndexDictionary(16) {}
//
//        explicit MSIIndexDictionary(int32 baseCapacity)
//            : exponent(0)
//            , threshold(0)
//            , size(0)
//            , pMap(nullptr)
//            , pKeys(nullptr)
//            , pValues(nullptr)
//            , values(baseCapacity) {
//            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
//            threshold = MSIMapUtil::InitBuffers(exponent, &pMap, &pKeys, &pValues);
//        }
//
//        ~MSIIndexDictionary() {
//            MSIMapUtil::Free(GetAllocation());
//        }
//
//        void GetKeyValues(CheckedArray<TKey> outKeys, CheckedArray<TValue> outValues) {
//            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
//            int32 keyIndex;
//            int32 idx = 0;
//            while (enumerator.MoveNext(&keyIndex)) {
//                outKeys.Set(idx, pKeys[keyIndex]);
//                outValues.Set(idx, values[pValues[keyIndex]]);
//                idx++;
//            }
//        }
//
//        void Set(TKey key, TValue value) {
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                // add as a new entry
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    pKeys[index] = key;
//                    pValues[index] = values.size;
//                    values.Add(value);
//                    size++;
//                    return;
//                }
//
//                // reuse the previous one and update the value
//                if (pKeys[index].Equals(key)) {
//                    values[pValues[index]] = value;
//                    return;
//                }
//            }
//        }
//
//        // todo -- this will insert a duplicate key!
//        void Add(TKey key, TValue value) {
//
//            if (size + 1 >= threshold) {
//                MSIMapUtil(&exponent, &threshold, &pMap, &pKeys, &pValues);
//            }
//
//            int32 h = key.GetHashCode();
//
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = (h + step) & mask;
//
//            while (!LongBoolMap::TrySetIndex(pMap, index)) {
//                index = (index + step) & mask;
//            }
//
//            pKeys[index] = key;
//            pValues[index] = values.size;
//            values.Add(value);
//            size++;
//
//        }
//
//        bool TryGetValue(TKey key, TValue* value) {
//
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    return false;
//                }
//
//                if (pKeys[index].Equals(key)) {
//                    *value = values.Get(pValues[index]);
//                    return true;
//                }
//            }
//        }
//
//        TValue* GetValueAtIndex(int32 index) {
//            return values.GetPointer(pValues[index]);
//        }
//
//        bool TryGetIndex(TKey key, int32* result) {
//
//            int32 h = key.GetHashCode();
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = h;
//
//            while (true) {
//                index = (index + step) & mask;
//
//                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
//                    return false;
//                }
//
//                if (pKeys[index].Equals(key)) {
//                    *result = index;
//                    return true;
//                }
//            }
//        }
//
//        uint8* GetAllocation() {
//            return (uint8*) pMap;
//        }
//
//        void GetKeys(CheckedArray<TKey> outKeys) {
//            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
//            int32 keyIndex;
//            int32 idx = 0;
//            while (enumerator.MoveNext(&keyIndex)) {
//                outKeys.Set(idx, pKeys[keyIndex]);
//                idx++;
//            }
//        }
//
//        int32 GetValues(CheckedArray<TValue> outValues) {
//            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
//            int32 keyIndex;
//            int32 idx = 0;
//            while (enumerator.MoveNext(&keyIndex)) {
//                outValues.Set(idx++, values[pValues[keyIndex]]);
//            }
//            return idx;
//        }
//
//        void Clear() {
//            MSIMapUtil::Clear(exponent, GetAllocation());
//            values.size = 0;
//            size = 0;
//        }
//
//    };
//
//    template<typename TKey, typename TValue>
//    struct MSIMultiMap {
//
//        int32 exponent;
//        int32 threshold;
//        int32 size;
//
//        // we use a map of ulongs (64 bits) to hold a true/false value for each slot as whether it is occupied
//        // we always have enough ulongs in the buffer to account for the total capacity of the data structure
//        uint64* pMap;
//        uint64* pGravestones;
//        TKey* pKeys;
//        TValue* pValues;
//
//        MSIMultiMap() : MSIMultiMap(32) {}
//
//        explicit MSIMultiMap(int32 baseCapacity)
//            : exponent(0)
//            , threshold(0)
//            , size(0)
//            , pMap(nullptr)
//            , pGravestones(nullptr)
//            , pKeys(nullptr)
//            , pValues(nullptr) {
//            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
//            threshold = MSIMapUtil::InitRemovableBuffers(exponent, &pMap, &pGravestones, &pKeys, &pValues);
//        }
//
//        ~MSIMultiMap() {
//            MSIMapUtil::Free(GetAllocation());
//        }
//
//        void Add(TKey key, TValue value) {
//            if (size + 1 >= threshold) {
//                MSIMapUtil::ResizeRemovable<TKey, TValue>(&exponent, &threshold, &pMap, &pGravestones, &pKeys, &pValues);
//            }
//
//            int32 h = key.GetHashCode();
//
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = (h + step) & mask;
//
//            while (!LongBoolMap::TrySetIndex(pMap, index)) {
//
//                if (LongBoolMap::TryUnsetIndex(pGravestones, index)) {
//                    break;
//                }
//
//                index = (index + step) & mask;
//            }
//
//            pKeys[index] = key;
//            pValues[index] = value;
//            size++;
//
//        }
//
//        int32 Remove(const TKey& key) {
//            int32 h = key.GetHashCode();
//
//            int32 mask = (1 << exponent) - 1;
//            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
//
//            int32 index = (h + step) & mask;
//            int32 removeCount = 0;
//
//            while (LongBoolMap::IsMapBitSet(pMap, index)) {
//
//                // skip grave stones
//                if (LongBoolMap::IsMapBitSet(pGravestones, index)) {
//                    index = (index + step) & mask;
//                    continue;
//                }
//
//                // leave the map bit set, set the gravestone bit
//                if (pKeys[index].Equals(key)) {
//                    LongBoolMap::SetMapBit(pGravestones, index);
//                    removeCount++;
//                }
//
//                index = (index + step) & mask;
//            }
//
//            size -= removeCount;
//            assert(size  >= 0);
//            return removeCount;
//        }
//
//        MSIIterator<TKey> GetIterator(const TKey& key) {
//            int32 hashCode = key.GetHashCode();
//            return MSIIterator<TKey>(
//                key,
//                hashCode,
//                (1 << exponent) - 1,
//                (hashCode >> (MSIMapUtil::k_StepSubtract - exponent)) | 1);
//        }
//
//        bool TryGetNext(MSIIterator<TKey>* msiIterator, TValue* value) {
//            int32 i = msiIterator->index;
//
//            while (true) {
//                i = (i + msiIterator->step) & msiIterator->mask;
//
//                if (!LongBoolMap::IsMapBitSet(pMap, i)) {
//                    return false;
//                }
//
//                // skip gravestones
//                if (LongBoolMap::IsMapBitSet(pGravestones, i)) {
//                    continue;
//                }
//
//                if (pKeys[i].Equals(msiIterator->key)) {
//                    msiIterator->index = i;
//                    *value = pValues[i];
//                    return true;
//                }
//            }
//        }
//
//        int32 CountForKey(TKey key) {
//            int32 count = 0;
//
//            MSIIterator<TKey> msiIterator = GetIterator(key);
//
//            TValue value;
//            while (TryGetNext(&msiIterator, &value)) {
//                count++;
//            }
//
//            return count;
//        }
//
//        uint8* GetAllocation() {
//            return (uint8*) pMap;
//        }
//
//        void Clear() {
//            MSIMapUtil::ClearRemovable<TKey>(exponent, GetAllocation());
//            size = 0;
//        }
//    };
//}

#pragma once

#include <cstring>
#include "../Collections/LongBoolMap.h"
#include "../Collections/PodList.h"
#include "../Allocation/PodAllocation.h"

namespace Alchemy {

    struct StringKey {

        char* cbuffer;
        int32 length;

        StringKey(char* buffer, int32 length) : cbuffer(buffer), length(length) {}

        int32 GetHashCode() const {

            // FNV-1a

            constexpr uint32 FNV_PRIME_32 = 16777619;
            constexpr uint32 OFFSET_BASIS_32 = 2166136261;

            uint32 hash = OFFSET_BASIS_32;

            for (size_t i = 0; i < length; ++i) {
                hash *= FNV_PRIME_32;
                hash ^= static_cast<uint32>(cbuffer[i]);
            }

            return static_cast<int32>(hash);
        }

        bool Equals(StringKey other) const {
            if (cbuffer == other.cbuffer) {
                return true;
            }

            if (length != other.length) {
                return false;
            }

            return memcmp(cbuffer, other.cbuffer, length) == 0;

        }

    };

    struct IntKey {

        int32 key;

        explicit IntKey(int32 key) : key(key) {}

        int32 GetHashCode() const {
            int32 x = key;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            return x;
        }

        bool Equals(IntKey other) const {
            return key == other.key;
        }

    };

    struct MSIMapUtil {

        static uint64 Align(size_t number, size_t alignment) {
            return ((number + (alignment - 1)) & (-alignment));
        }

        static constexpr float k_FillFactor = 0.5f;
        static constexpr int k_StepSubtract = 32;

        static constexpr size_t k_Alignment = 16;

        template<typename TKey, typename TValue>
        static int32 InitBuffers(int32 exponent, uint64** pNewMap, TKey** pNewKeys, TValue** pNewValues) {
            int32 shiftedExponent = 1 << exponent;

            int64 mapSize = (int64) Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
            int64 keySize = (int64) Align(sizeof(TKey) * shiftedExponent, 8);
            int64 valueSize = (int64) Align(sizeof(TValue) * shiftedExponent, 8);

            uint8* bytes = Allocate(mapSize + keySize + valueSize);

            // need to clear keys in case we compare with a key value that randomly
            // happens to be a valid value in memory but isn't a real key.
            // we don't clear values since they only get read one success

            memset(bytes, 0, mapSize + keySize);

            *pNewMap = (uint64*) bytes;
            *pNewKeys = (TKey*) (bytes + mapSize);
            *pNewValues = (TValue*) (bytes + mapSize + keySize);

            return (int32) (shiftedExponent * k_FillFactor);
        }

        static uint8* Allocate(size_t size) {
            return (uint8*) MallocByteArray(size, k_Alignment);
        }

        template<typename TKey, typename TValue>
        static int32 InitRemovableBuffers(int32 exponent, uint64** pNewMap, uint64** pNewGravestones, TKey** pNewKeys, TValue** pNewValues) {
            int32 shiftedExponent = 1 << exponent;

            int64 mapSize = (int64) Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
            int64 keySize = (int64) Align(sizeof(TKey) * shiftedExponent, 8);
            int64 valueSize = (int64) Align(sizeof(TValue) * shiftedExponent, 8);

            uint8* bytes = Allocate(mapSize + mapSize + keySize + valueSize);

            // need to clear keys in case we compare with a key value that randomly
            // happens to be a valid value in memory but isn't a real key.
            // we don't clear values since they only get read one success

            memset(bytes, 0, mapSize + mapSize + keySize);

            *pNewMap = (uint64*) bytes;
            *pNewGravestones = (uint64*) (bytes + mapSize);
            *pNewKeys = (TKey*) (bytes + mapSize + mapSize);
            *pNewValues = (TValue*) (bytes + mapSize + mapSize + keySize);

            return (int32) (shiftedExponent * k_FillFactor);
        }

        template<typename TKey>
        static void Clear(int32 exponent, void* allocation) {
            int32 shiftedExponent = 1 << exponent;

            int64 mapSize = Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
            int64 keySize = Align(sizeof(TKey) * shiftedExponent, 8);

            // need to clear keys in case we compare with a key value that randomly
            // happens to be a valid value in memory but isn't a real key.
            // we don't clear values since they only get read one success
            memset(allocation, 0, mapSize + keySize);

        }

        template<typename TKey>
        static void ClearRemovable(int32 exponent, void* allocation) {
            int32 shiftedExponent = 1 << exponent;

            int64 mapSize = Align(sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent), 8);
            int64 keySize = Align(sizeof(TKey) * shiftedExponent, 8);

            // need to clear keys in case we compare with a key value that randomly
            // happens to be a valid value in memory but isn't a real key.
            // we don't clear values since they only get read one success
            memset(allocation, 0, mapSize + mapSize + keySize);

        }

        static void Free(void* memory) {
            FreeByteArray(memory, 16);
        }

        template<typename TKey, typename TValue>
        static void Resize(int32* exponent, int32* threshold, uint64** pMap, TKey** pKeys, TValue** pValues) {

            // Note: if we resize this we end up changing the order of the keys, which means we lose the guarantee
            // that values are returned in the order that the keys are added! In order to actually support that use case
            // we would need to keep an additional ordered list of keys. I've chosen not to do this. If the caller needs the values
            // back in a certain order they should just sort the list themselves.

            LongBoolMapEnumerator mapIterator(*pMap, LongBoolMap::GetMapSize(1 << *exponent));
            *exponent = *exponent + 1; // bump exponent, capacity is 1 << exponent

            uint64* pNewMap;
            TKey* pNewKeys;
            TValue* pNewValues;

            *threshold = MSIMapUtil::InitBuffers(*exponent, &pNewMap, &pNewKeys, &pNewValues);

            int32 mask = (1 << *exponent) - 1;

            TKey* prevKeys = *pKeys;
            TValue* prevValues = *pValues;
            while (mapIterator.MoveNext()) {
                int32 prevIndex = mapIterator.Current();
                int32 hashCode = prevKeys[prevIndex].GetHashCode();
                int32 step = hashCode >> (k_StepSubtract - *exponent) | 1;

                int32 index = (hashCode + step) & mask;

                while (!LongBoolMap::TrySetIndex(pNewMap, index)) {
                    index = (index + step) & mask;
                }

                memcpy(&pNewKeys[index], &prevKeys[prevIndex], sizeof(TKey));
                memcpy(&pNewValues[index], &prevValues[prevIndex], sizeof(TValue));

            }

            Free(*pMap);

            *pMap = pNewMap;
            *pKeys = pNewKeys;
            *pValues = pNewValues;

        }

        template<typename TKey, typename TValue>
        static void ResizeRemovable(int32* exponent, int32* threshold, uint64** pMap, uint64** pGravestones, TKey** pKeys, TValue** pValues) {

            // Note: if we resize this we end up changing the order of the keys, which means we lose the guarantee
            // that values are returned in the order that the keys are added! In order to actually support that use case
            // we would need to keep an additional ordered list of keys. I've chosen not to do this. If the caller needs the values
            // back in a certain order they should just sort the list themselves.

            int32 mapSize = LongBoolMap::GetMapSize(1 << *exponent);
            *exponent = *exponent + 1; // bump exponent, capacity is 1 << exponent

            uint64* pNewMap;
            uint64* pNewGravestones;
            TKey* pNewKeys;
            TValue* pNewValues;

            *threshold = MSIMapUtil::InitRemovableBuffers(*exponent, &pNewMap, &pNewGravestones, &pNewKeys, &pNewValues);

            int32 mask = (1 << *exponent) - 1;

            TKey* prevKeys = *pKeys;
            TValue* prevValues = *pValues;
            uint64* prevMap = *pMap;
            uint64* prevGravestones = *pGravestones;

            for (int32 i = 0; i < mapSize; i++) {
                uint64 combined = prevMap[i] & ~prevGravestones[i];
                LongBoolMapEnumerator mapIterator(&combined, 1);

                while (mapIterator.MoveNext()) {
                    int32 prevIndex = (i * 64) + mapIterator.Current();
                    int32 hashCode = prevKeys[prevIndex].GetHashCode();
                    int32 step = hashCode >> (k_StepSubtract - *exponent) | 1;

                    int32 index = (hashCode + step) & mask;

                    while (!LongBoolMap::TrySetIndex(pNewMap, index)) {
                        index = (index + step) & mask;
                    }

                    memcpy(&pNewKeys[index], &prevKeys[prevIndex], sizeof(TKey));
                    memcpy(&pNewValues[index], &prevValues[prevIndex], sizeof(TValue));

                }

            }

            Free(*pMap);

            *pMap = pNewMap;
            *pGravestones = pNewGravestones;
            *pKeys = pNewKeys;
            *pValues = pNewValues;

        }
    };

    template<typename TKey>
    struct MSIIterator {

        TKey key;
        int32 index;
        int32 mask;
        int32 step;

        explicit MSIIterator(TKey key, int32 index, int32 mask, int32 step) : key(key), index(index), mask(mask), step(step) {}

    };

    template<typename TKey, typename TValue>
    struct MSIDictionary {
        int32 exponent;
        int32 threshold;
        int32 size;
        uint64* pMap;
        TKey* pKeys;
        TValue* pValues;

        MSIDictionary() : MSIDictionary(16) {}

        explicit MSIDictionary(int32 baseCapacity)
                : exponent(0)
                  , threshold(0)
                  , size(0)
                  , pMap(nullptr)
                  , pKeys(nullptr)
                  , pValues(nullptr) {
            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
            threshold = MSIMapUtil::InitBuffers(exponent, &pMap, &pKeys, &pValues);
        }

        ~MSIDictionary() {
            MSIMapUtil::Free(GetAllocation());
        }

        void Set(TKey key, TValue value) {
            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                // add as a new entry
                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    pKeys[index] = key;
                    pValues[index] = value;
                    if (size + 1 >= threshold) {
                        MSIMapUtil::Resize<TKey, TValue>(&exponent, &threshold, &pMap, &pKeys, &pValues);
                    }
                    size++;
                    return;
                }

                // reuse the previous one and update the value
                if (pKeys[index].Equals(key)) {
                    pValues[index] = value;
                    return;
                }
            }
        }

        bool Contains(TKey key) {
            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    return false;
                }

                if (pKeys[index].Equals(key)) {
                    return true;
                }

            }

        }

        int32 GetValues(CheckedArray<TValue> outValues) {
            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
            int32 keyIndex;
            int32 idx = 0;
            while (enumerator.MoveNext(&keyIndex)) {
                outValues.Set(idx++, pValues[keyIndex]);
            }
            return idx;
        }

        bool TryGetValue(TKey key, TValue* value) {

            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    return false;
                }

                if (pKeys[index].Equals(key)) {
                    *value = pValues[index];
                    return true;
                }

            }
        }

        bool TryAdd(TKey key, const TValue & value) {

            // optimistically assume we'll add it, resize if we can't handle it
            if (size + 1 >= threshold) {
                MSIMapUtil::Resize<TKey, TValue>(&exponent, &threshold, &pMap, &pKeys, &pValues);
            }

            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                if (LongBoolMap::TrySetIndex(pMap, index)) {
                    pKeys[index] = key;
                    pValues[index] = value;
                    size++;
                    return true;
                }

                if (pKeys[index].Equals(key)) {
                    return false;
                }

            }

        }

        uint8* GetAllocation() {
            return (uint8*) pMap;
        }

        void Clear() {
            int32 shiftedExponent = 1 << exponent;

            // need to clear keys in case we compare with a key value that randomly
            // happens to be a valid value in memory but isn't a real key.
            // we don't clear values since they only get read one success
            size = 0;

            memset(pMap, 0, sizeof(uint64) * LongBoolMap::GetMapSize(shiftedExponent));
            memset(pKeys, 0, sizeof(TKey) * shiftedExponent);

        }

    };


    template<typename TKey>
    struct MSISet { // todo -- make a real set w/o value allocations-

        MSIDictionary<TKey, uint8> dictionary;

        explicit MSISet(int32 initialCapacity)
                : dictionary(initialCapacity)
        {}

        void Initialize(int32 capacity) {
            dictionary.Initialize(capacity);
        }

        bool TryAdd(TKey key){
            return dictionary.TryAdd(key, 0);
        }

        bool Contains(TKey key) {
            return dictionary.Contains(key);
        }

        int32 GetSize() const {
            return dictionary.size;
        }

        void Clear() {
            dictionary.Clear();
        }

        void GetKeys(CheckedArray<TKey> outKeys) {
            LongBoolMapEnumerator enumerator(dictionary.pMap, LongBoolMap::GetMapSize(1 << dictionary.exponent));
            int32 keyIndex;
            int32 idx = 0;
            while (enumerator.MoveNext(&keyIndex)) {
                outKeys.Set(idx, dictionary.pKeys[keyIndex]);
                idx++;
            }
        }

    };

    template<typename TKey, typename TValue>
    struct MSIIndexDictionary {

        int32 exponent;
        int32 threshold;
        int32 size;
        PodList<TValue> values;
        uint64* pMap;
        TKey* pKeys;
        int32* pValues;

        MSIIndexDictionary() : MSIIndexDictionary(16) {}

        explicit MSIIndexDictionary(int32 baseCapacity)
                : exponent(0)
                  , threshold(0)
                  , size(0)
                  , pMap(nullptr)
                  , pKeys(nullptr)
                  , pValues(nullptr)
                  , values(baseCapacity) {
            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
            threshold = MSIMapUtil::InitBuffers(exponent, &pMap, &pKeys, &pValues);
        }

        ~MSIIndexDictionary() {
            MSIMapUtil::Free(GetAllocation());
        }

        void GetKeyValues(CheckedArray<TKey> outKeys, CheckedArray<TValue> outValues) {
            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
            int32 keyIndex;
            int32 idx = 0;
            while (enumerator.MoveNext(&keyIndex)) {
                outKeys.Set(idx, pKeys[keyIndex]);
                outValues.Set(idx, values[pValues[keyIndex]]);
                idx++;
            }
        }

        void Set(TKey key, TValue value) {
            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;

            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                // add as a new entry
                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    pKeys[index] = key;
                    pValues[index] = values.size;
                    values.Add(value);
                    size++;
                    return;
                }

                // reuse the previous one and update the value
                if (pKeys[index].Equals(key)) {
                    values[pValues[index]] = value;
                    return;
                }
            }
        }

        // todo -- this will insert a duplicate key!
        void Add(TKey key, TValue value) {

            if (size + 1 >= threshold) {
                MSIMapUtil::Resize<TKey, int32>(&exponent, &threshold, &pMap, &pKeys, &pValues);
            }

            int32 h = key.GetHashCode();

            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;

            int32 index = (h + step) & mask;

            while (!LongBoolMap::TrySetIndex(pMap, index)) {
                index = (index + step) & mask;
            }

            pKeys[index] = key;
            pValues[index] = values.size;
            values.Add(value);
            size++;

        }

        bool TryGetValue(TKey key, TValue* value) {

            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;
            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    return false;
                }

                if (pKeys[index].Equals(key)) {
                    *value = values.Get(pValues[index]);
                    return true;
                }
            }
        }

        TValue* GetValueAtIndex(int32 index) {
            return values.GetPointer(pValues[index]);
        }

        bool TryGetIndex(TKey key, int32* result) {

            int32 h = key.GetHashCode();
            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;

            int32 index = h;

            while (true) {
                index = (index + step) & mask;

                if (!LongBoolMap::IsMapBitSet(pMap, index)) {
                    return false;
                }

                if (pKeys[index].Equals(key)) {
                    *result = index;
                    return true;
                }
            }
        }

        uint8* GetAllocation() {
            return (uint8*) pMap;
        }

        void GetKeys(CheckedArray<TKey> outKeys) {
            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
            int32 keyIndex;
            int32 idx = 0;
            while (enumerator.MoveNext(&keyIndex)) {
                outKeys.Set(idx, pKeys[keyIndex]);
                idx++;
            }
        }

        int32 GetValues(CheckedArray<TValue> outValues) {
            LongBoolMapEnumerator enumerator(pMap, LongBoolMap::GetMapSize(1 << exponent));
            int32 keyIndex;
            int32 idx = 0;
            while (enumerator.MoveNext(&keyIndex)) {
                outValues.Set(idx++, values[pValues[keyIndex]]);
            }
            return idx;
        }

        void Clear() {
            MSIMapUtil::Clear<TKey>(exponent, GetAllocation());
            values.size = 0;
            size = 0;
        }

    };

    template<typename TKey, typename TValue>
    struct MSIMultiMap {

        int32 exponent;
        int32 threshold;
        int32 size;

        // we use a map of ulongs (64 bits) to hold a true/false value for each slot as whether it is occupied
        // we always have enough ulongs in the buffer to account for the total capacity of the data structure
        uint64* pMap;
        uint64* pGravestones;
        TKey* pKeys;
        TValue* pValues;

        MSIMultiMap() : MSIMultiMap(32) {}

        explicit MSIMultiMap(int32 baseCapacity)
                : exponent(0)
                  , threshold(0)
                  , size(0)
                  , pMap(nullptr)
                  , pGravestones(nullptr)
                  , pKeys(nullptr)
                  , pValues(nullptr) {
            exponent = tzcnt32(MathUtil::CeilPow2(16 > baseCapacity ? 16 : baseCapacity));
            threshold = MSIMapUtil::InitRemovableBuffers(exponent, &pMap, &pGravestones, &pKeys, &pValues);
        }

        ~MSIMultiMap() {
            MSIMapUtil::Free(GetAllocation());
        }

        void Add(TKey key, TValue value) {
            if (size + 1 >= threshold) {
                MSIMapUtil::ResizeRemovable<TKey, TValue>(&exponent, &threshold, &pMap, &pGravestones, &pKeys, &pValues);
            }

            int32 h = key.GetHashCode();

            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;

            int32 index = (h + step) & mask;

            while (!LongBoolMap::TrySetIndex(pMap, index)) {

                if (LongBoolMap::TryUnsetIndex(pGravestones, index)) {
                    break;
                }

                index = (index + step) & mask;
            }

            pKeys[index] = key;
            pValues[index] = value;
            size++;

        }

        int32 Remove(const TKey& key) {
            int32 h = key.GetHashCode();

            int32 mask = (1 << exponent) - 1;
            int32 step = (h >> (MSIMapUtil::k_StepSubtract - exponent)) | 1;

            int32 index = (h + step) & mask;
            int32 removeCount = 0;

            while (LongBoolMap::IsMapBitSet(pMap, index)) {

                // skip grave stones
                if (LongBoolMap::IsMapBitSet(pGravestones, index)) {
                    index = (index + step) & mask;
                    continue;
                }

                // leave the map bit set, set the gravestone bit
                if (pKeys[index].Equals(key)) {
                    LongBoolMap::SetMapBit(pGravestones, index);
                    removeCount++;
                }

                index = (index + step) & mask;
            }

            size -= removeCount;
            assert(size  >= 0);
            return removeCount;
        }

        MSIIterator<TKey> GetIterator(const TKey& key) {
            int32 hashCode = key.GetHashCode();
            return MSIIterator<TKey>(
                    key,
                    hashCode,
                    (1 << exponent) - 1,
                    (hashCode >> (MSIMapUtil::k_StepSubtract - exponent)) | 1);
        }

        bool TryGetNext(MSIIterator<TKey>* msiIterator, TValue* value) {
            int32 i = msiIterator->index;

            while (true) {
                i = (i + msiIterator->step) & msiIterator->mask;

                if (!LongBoolMap::IsMapBitSet(pMap, i)) {
                    return false;
                }

                // skip gravestones
                if (LongBoolMap::IsMapBitSet(pGravestones, i)) {
                    continue;
                }

                if (pKeys[i].Equals(msiIterator->key)) {
                    msiIterator->index = i;
                    *value = pValues[i];
                    return true;
                }
            }
        }

        int32 CountForKey(TKey key) {
            int32 count = 0;

            MSIIterator<TKey> msiIterator = GetIterator(key);

            TValue value;
            while (TryGetNext(&msiIterator, &value)) {
                count++;
            }

            return count;
        }

        uint8* GetAllocation() {
            return (uint8*) pMap;
        }

        void Clear() {
            MSIMapUtil::ClearRemovable<TKey>(exponent, GetAllocation());
            size = 0;
        }
    };
}