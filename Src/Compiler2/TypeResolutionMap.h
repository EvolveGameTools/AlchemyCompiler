#pragma once

#include "../Allocation/LinearAllocator.h"
#include "./TypeInfo.h"
#include "../Util/Hash.h"
#include "./ResolvedType.h"
#include <mutex>

namespace Alchemy::Compilation {

    struct GenericReplacement {
        FixedCharSpan genericName;
        ResolvedType resolvedGeneric;
    };

    struct TypeResolutionMap {

        explicit TypeResolutionMap(Allocator allocator);

        bool AddUnlocked(TypeInfo * typeInfo);
        bool AddLocked(TypeInfo * typeInfo);

        CheckedArray<TypeInfo*> GetValues(Allocator allocator);

        void ReplaceValues(CheckedArray<TypeInfo*> array);

        int32 GetLongestEntrySize();

        bool TryResolve(FixedCharSpan span, TypeInfo** pInfo);

        ResolvedType MakeGenericType(TypeInfo* openType, CheckedArray <ResolvedType> typeArguments, Allocator typeAllocator);

        CheckedArray<TypeInfo*> builtInTypeInfos;

        TypeInfo * unresolvedType;
        TypeInfo * voidType;

        FixedCharSpan DumpTypeTable(Allocator dumpAllocator);


    private:

        Allocator allocator;
        CheckedArray<TypeInfo*> values;
        std::mutex mutex;
        int32 exponent;
        int32 size;
        int32 longestEntrySize;

        void ResizeTable();

        // returns true if added, existing entries are not overridden
        bool AddInternal(TypeInfo* typeInfo);

        ResolvedType RecursiveResolveGenerics(ResolvedType input, CheckedArray<GenericReplacement> replacements, Allocator & alloc);

    };


}