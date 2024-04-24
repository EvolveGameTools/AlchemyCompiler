#pragma once

#include "../Parsing/Parser.h"
#include "../Collections/PodList.h"
#include "../Allocation/LinearAllocator.h"
#include "./TypeInfo.h"
#include "PooledBlockAllocator.h"

namespace Alchemy::Compilation {

    struct TypeContext;

    struct DeclaredType {
        TypeInfo* typeInfo{};
        TypeContext* typeContext{};
    };

    struct GenericTypeCache;

    GenericTypeCache* MakeGenericTypeCache(LinearAllocator* allocator);

    void ClearGenericTypeCache(GenericTypeCache* cache);

    void DestroyGenericTypeCache(GenericTypeCache* cache);

    TypeInfo* MakeGenericType(GenericTypeCache* cache, TypeInfo* openType, CheckedArray<ResolvedType> typeArguments, TempAllocator * tempAllocator);
    MethodInfo * GenericTypeCache_MakeGenericMethod(GenericTypeCache * cache, MethodInfo * methodInfo, CheckedArray<ResolvedType> resolvedGenerics);

    struct FileInfo {

        ParseResult parseResult; // todo -- would be better to allocate the lists via our linear allocator
        int32 fileId;
        bool wasChanged;
        FixedCharSpan packageName;
        FixedCharSpan filePath;
        LinearAllocator allocator;
        CheckedArray<DeclaredType> declaredTypes;
        CheckedArray<FixedCharSpan> usingNamespaces;
        CheckedArray<UsingAlias> usingAliases;
        CheckedArray<uint64> fileReferenceMap;
        uint64 lastEditTime;

        PoolingAllocator poolingAllocator;

        FileInfo(FixedCharSpan &packageName_, FixedCharSpan &filePath_)
                : allocator(GIGABYTES(1), KILOBYTES(16))
                  , fileId(0)
                  , packageName()
                  , wasChanged(true)
                  , filePath()
                  , poolingAllocator(&allocator) {
            filePath = FixedCharSpan(allocator.AllocateUncleared<char>(filePath_.size + 1), filePath_.size);
            packageName = FixedCharSpan(allocator.AllocateUncleared<char>(packageName_.size + 1), packageName_.size);
            memcpy(packageName.ptr, packageName_.ptr, packageName_.size);
            memcpy(filePath.ptr, filePath_.ptr, filePath_.size);
            filePath.ptr[filePath.size] = '\0';
            packageName.ptr[packageName.size] = '\0';
        }

        ~FileInfo() {
            Invalidate();
        }

        void Dispose() {
            Invalidate();
        }

        void Invalidate() {
            fileReferenceMap.size = 0;
            allocator.Clear();
            declaredTypes.size = 0;
            usingNamespaces.size = 0;
            usingAliases.size = 0;
        }

        void EnsureFileCount(int32 fileCount) {
            int32 requiredSize = LongBoolMap::GetMapSize(fileCount);
            int32 size = fileReferenceMap.size;

            if (size < requiredSize) {
                uint64* map = poolingAllocator.AllocateUncleared<uint64>(requiredSize);
                memcpy(fileReferenceMap.array, map, sizeof(uint64) * fileReferenceMap.size);
                poolingAllocator.Free(fileReferenceMap.array, fileReferenceMap.size);
                for (int32 i = size; i < requiredSize; i++) {
                    map[i] = 0;
                }
                fileReferenceMap.array = map;
                fileReferenceMap.size = requiredSize;
            }

        }

        FixedCharSpan GetSourceRange(uint16 tokenStart, uint16 tokenEnd) {
            int32 start = parseResult.tokens[tokenStart].position;
            int32 end = parseResult.tokens[tokenEnd - 1].position;

            return FixedCharSpan(parseResult.src + start, end - start);
        }

        void AddFileReference(FileInfo* fileInfo) {
            LongBoolMap(fileReferenceMap.array, fileReferenceMap.size).Set(fileInfo->fileId);
        }

        LongBoolMap GetDependencyMap() {
            return LongBoolMap(fileReferenceMap.array, fileReferenceMap.size);
        }

    };

    bool TypeInfoIsReferenceType(TypeInfo* typeInfo) {

        if (typeInfo == nullptr) {
            return false;
        }

        return typeInfo->typeClass == TypeClass::Delegate || typeInfo->typeClass == TypeClass::Class || typeInfo->typeClass == TypeClass::Interface;

    }

    FixedCharSpan GetDeclaringFilePathImpl(TypeInfo* typeInfo) {
        if (typeInfo == nullptr) {
            return FixedCharSpan();
        }
        return typeInfo->declaringFile->filePath;
    }
}