
#include "./StringTable.h"
#include "../Util/Hash.h"
#include "../Util/MathUtil.h"

namespace Alchemy {

    StringTable::StringTable(Allocator allocator, int32 initialCapacity)
        : allocator(allocator)
        , size(0) {

        if (initialCapacity < 128) {
            initialCapacity = 128;
        }

        int32 pow2 = MathUtil::CeilPow2(initialCapacity);
        exponent = MathUtil::LogPow2(pow2);
        table = MallocateTyped(FixedCharSpan, pow2);

    }

    StringTable::~StringTable() {

        int32 total = 1 << exponent;

        for (int32 i = 0; i < total; i++) {
            if (table[i].ptr != nullptr) {
                allocator.Free(table[i].ptr, table[i].size + 1);
            }
        }

        MfreeTyped(table, total);

    }

    FixedCharSpan StringTable::Intern(FixedCharSpan toIntern) {
        int32 h = MsiHash::FNV1a((char*) toIntern.ptr, toIntern.size);

        // std::unique_lock lock(internMutex); // not sure how to improve this but this isn't ideal

        for (int32 i = h;;) {
            i = MsiHash::Lookup32(h, exponent, i);

            FixedCharSpan tableSpan = table[i];

            if (tableSpan.ptr == nullptr) {

                char* c = allocator.AllocateUncleared<char>(toIntern.size + 1);

                memcpy(c, toIntern.ptr, toIntern.size * sizeof(char));
                c[toIntern.size] = '\0';

                FixedCharSpan allocated(c, toIntern.size);

                // allow the table to fill up to 50% of the values
                int32 threshold = (1 << exponent) >> 1;

                // add no matter what, if we've exceeded the threshold that's ok since we have extra space
                table[i] = FixedCharSpan(c, toIntern.size);
                size++;

                // if we're still under the threshold, return what we just allocated
                if (size <= threshold) {
                    return allocated;
                }

                // otherwise we need to re-hash the table
                int32 newExponent = exponent + 1;

                FixedCharSpan* newList = MallocateTyped(FixedCharSpan, 1 << newExponent);

                int32 previousTotalSize = 1 << exponent;

                for (int32 s = 0; s < previousTotalSize; s++) {
                    FixedCharSpan x = table[s];
                    if (x.ptr == nullptr) {
                        continue;
                    }
                    int32 h2 = MsiHash::FNV1a((char*) x.ptr, x.size);
                    int32 i2 = h2;
                    while (true) {
                        i2 = MsiHash::Lookup32(h2, newExponent, i2);
                        if (newList[i2].ptr == nullptr) {
                            newList[i2] = x;
                            break;
                        }
                    }
                }

                MfreeTyped(table, previousTotalSize);
                exponent = newExponent;
                table = newList;

            }
            else if (tableSpan == toIntern) {
                // found it
                return tableSpan;
            }

        }

    }
}