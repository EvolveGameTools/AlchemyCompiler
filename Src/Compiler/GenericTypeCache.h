#pragma once

#include "../Collections/MSIHashmap.h"
#include "../Allocation/PoolAllocator.h"

namespace Alchemy::Compilation {

    struct GenericTypeCache {

        MSIDictionary<FixedCharSpan, TypeInfo*> cache;

        std::mutex mtx;

        GenericTypeCache() : cache(128), mtx() {}

        ResolvedType RecursiveResolveGenerics(ResolvedType input, CheckedArray<GenericReplacement> replacements, TempAllocator* tempAllocator) {

            // simple type name reference, no work to do
            if (input.typeInfo == nullptr) {
                return input;
            }

            if ((input.typeInfo->flags & TypeInfoFlags::IsGeneric) != 0) {
                FixedCharSpan typeName = input.typeInfo->GetTypeName();
                for (int32 i = 0; i < replacements.size; i++) {
                    if (replacements[i].genericName == typeName) {
                        return replacements[i].resolvedGeneric;
                    }
                }
                assert(false && "unreachable");
            }

            if ((input.typeInfo->flags & TypeInfoFlags::ContainsOpenGenericTypes) != 0) {

                ResolvedType retn = input; // copy flags

                int32 cnt = input.typeInfo->genericArgumentCount;

                CheckedArray<ResolvedType> replacedArgs(tempAllocator->AllocateUncleared<ResolvedType>(cnt), cnt);

                for (int32 i = 0; i < cnt; i++) {
                    // for each generic type argument, return a replacement of it
                    replacedArgs[i] = RecursiveResolveGenerics(input.typeInfo->genericArguments[i], replacements, tempAllocator);
                }

                retn.typeInfo = MakeGenericType(input.typeInfo, replacedArgs, tempAllocator);
                return retn;

            }

            return input;

        }

        TypeInfo* MakeGenericType(TypeInfo* openType, CheckedArray<ResolvedType> typeArguments, TempAllocator* tempAllocator) {

            assert(openType != nullptr);
            assert(openType->IsGenericTypeDefinition());
            assert(openType->genericArgumentCount == typeArguments.size);

            FixedCharSpan fullyQualifiedName = openType->GetFullyQualifiedTypeName(); // returns typename`genericCount w/o <types>

            int32 nameSize = fullyQualifiedName.size + 2; // 2 for < >

            for (int32 i = 0; i < typeArguments.size; i++) {
                nameSize += typeArguments[i].ToStringCount();
                if (i != typeArguments.size - 1) {
                    nameSize += 1; // ,
                }
            }

            // we end up with a string like System::Collections::List`1<float>

            TempAllocator::ScopedMarker scopedMarker(tempAllocator);

            char* p = tempAllocator->Allocate<char>(nameSize + 1);
            char* tempNameLookup = p;

            memcpy(p, fullyQualifiedName.ptr, fullyQualifiedName.size);
            p += fullyQualifiedName.size;
            *p++ = '<';
            for (int32 i = 0; i < typeArguments.size; i++) {
                p += typeArguments[i].ToString(p);
                if (i != typeArguments.size - 1) {
                    *p = ',';
                }
            }
            *p++ = '>';
            *p = '\0';

            FixedCharSpan lookup(tempNameLookup, nameSize);

            TypeInfo* result;

            {
                // if we already created this type, return it
                std::unique_lock lock(mtx);
                if (cache.TryGetValue(lookup, &result)) {
                    return result;
                }
            }

            // if base type is generic we'll need to make a generic base type
            // same w/ interfaces
            // same w/ constraints too
            // if base type are not generic we can stop

            // field/property infos need to point to their declaring type
            // we need that to figure out which types get touched for codegen
            // are there holes in that logic?
            // subclasses for example? no, we don't generate subclasses if not used
            // we always generate fields (and properties which are backed by fields) we can maybe omit untouched fields if the type is a leaf type
            // we only generate methods when actually used.

            size_t totalSize = sizeof(TypeInfo) +
                    openType->baseTypeCount * sizeof(TypeInfo*) +
                    openType->fieldCount * sizeof(FieldInfo) +
                    openType->propertyCount * sizeof(PropertyInfo) +
                    openType->methodCount * sizeof(MethodInfo) +
                    openType->indexerCount * sizeof(IndexerInfo) +
                    openType->constructorCount * sizeof(ConstructorInfo) +
                    openType->constraintCount * sizeof(GenericConstraint) +
                    openType->genericArgumentCount * sizeof(ResolvedType) +
                    (nameSize + 1) * sizeof(char);

            char* memoryBlock = (char*)malloc(totalSize);

            TypeInfo* newType = (TypeInfo*)memoryBlock;

            memcpy(newType, openType, sizeof(TypeInfo));

            memoryBlock += sizeof(TypeInfo);

            newType->baseType = (TypeInfo**)memoryBlock;
            memoryBlock += newType->baseTypeCount * sizeof(TypeInfo*);

            newType->fields = (FieldInfo*)memoryBlock;
            memoryBlock += newType->fieldCount * sizeof(FieldInfo);

            newType->properties = (PropertyInfo*)memoryBlock;
            memoryBlock += newType->propertyCount * sizeof(PropertyInfo);

            newType->methods = (MethodInfo*)memoryBlock;
            memoryBlock += newType->methodCount * sizeof(MethodInfo);

            newType->indexers = (IndexerInfo*)memoryBlock;
            memoryBlock += newType->indexerCount * sizeof(IndexerInfo);

            newType->constructors = (ConstructorInfo*)memoryBlock;
            memoryBlock += newType->constructorCount * sizeof(ConstructorInfo);

            newType->constraints = (GenericConstraint*)memoryBlock;
            memoryBlock += newType->constraintCount * sizeof(GenericConstraint);

            newType->genericArguments = (ResolvedType*)memoryBlock;
            memoryBlock += newType->genericArgumentCount * sizeof(ResolvedType);

            char* allocatedNameLookup = (char*)memoryBlock;
            memcpy(allocatedNameLookup, tempNameLookup, nameSize + 1); // + 1 copies terminator

            lookup = FixedCharSpan(allocatedNameLookup, nameSize);

            CheckedArray<ResolvedType> openGenerics = openType->GetGenericArguments();
            CheckedArray<GenericReplacement> replacements(tempAllocator->AllocateUncleared<GenericReplacement>(typeArguments.size), typeArguments.size);

            for(int32 i = 0; i < typeArguments.size; i++) {
                replacements[i].genericName = openGenerics[i].typeInfo->GetTypeName();
                replacements[i].resolvedGeneric = typeArguments[i];
            }

            for(int32 i = 0; i < newType->genericArgumentCount; i++) {
                 newType->genericArguments[i] = typeArguments[i];
            }

            for (int32 i = 0; i < openType->baseTypeCount; i++) {
                newType->baseType[i] = RecursiveResolveGenerics(ResolvedType(openType->baseType[i]), replacements, tempAllocator).typeInfo;
            }

            for(int32 i = 0; i < openType->fieldCount; i++) {
                newType->fields[i] = openType->fields[i];
                newType->fields[i].declaringType = newType;
                newType->fields[i].type = RecursiveResolveGenerics(newType->fields[i].type, replacements, tempAllocator);
            }

            for(int32 i = 0; i < openType->propertyCount; i++) {
                newType->properties[i] = openType->properties[i];
                newType->properties[i].declaringType = newType;
                newType->properties[i].type = RecursiveResolveGenerics(newType->properties[i].type, replacements, tempAllocator);
            }

            for(int32 i = 0; i < openType->methodCount; i++) {

                memcpy(&newType->methods[i], &openType->methods[i], sizeof(MethodInfo));

                MethodInfo * methodInfo = &newType->methods[i];
                methodInfo->declaringType = newType;
                methodInfo->returnType = RecursiveResolveGenerics(openType->methods[i].returnType, replacements, tempAllocator);

                for(int32 paramIndex = 0; paramIndex < methodInfo->parameterCount; paramIndex++) {
                    methodInfo->parameters[paramIndex].type = RecursiveResolveGenerics(methodInfo->parameters[paramIndex].type, replacements, tempAllocator);
                }

            }

            newType->flags &= ~TypeInfoFlags::IsGenericTypeDefinition;
            newType->flags |= TypeInfoFlags::InstantiatedGeneric;

            {
                std::unique_lock lock(mtx);
                TypeInfo * retn = nullptr;
                // in the time we took to create the type data, its possible another thread already created the type and registered it
                if(cache.TryGetValue(lookup, &retn)) {
                    free(newType);
                    return retn;
                }

                assert(cache.TryAdd(lookup, newType));

                return newType;

            }

        }

        MethodInfo* MakeGenericMethod(MethodInfo * methodInfo, CheckedArray<ResolvedType> argumentTypes) {
            // type_path::MethodName(parameters) -> return type
//            FixedCharSpan declaringTypeName = methodInfo->declaringType->GetFullyQualifiedTypeName();
//            int32 cnt = declaringTypeName.size;
//
//            MethodInfo * newMethod = allocator->AllocateUncleared<MethodInfo>(1);
//
//            *newMethod = *methodInfo;
//            newMethod->isGenericDefinition = false;
//            // not sure how to handle
//
//            for(int32 i = 0; i < methodInfo->parameterCount; i++) {
//                // do we copy the parameter types or make a new list?
//                //methodInfo->parameters[i];
//
//            }
//
//            if(newMethod->returnType.IsOrContainsGeneric()) {
//
//            }
//
//            assert(false && "not implemented");
            return nullptr;

        }

    };

    void GenericTypeCache_Destroy(GenericTypeCache * cache, TempAllocator * allocator) {
        TempAllocator::ScopedMarker marker(allocator);
        TypeInfo** pTypeInfos = allocator->AllocateUncleared<TypeInfo*>(cache->cache.size);
        CheckedArray<TypeInfo*> typeInfos(pTypeInfos, cache->cache.size);
        cache->cache.GetValues(typeInfos);

        for(int32 i = 0; i < typeInfos.size; i++) {
            free(typeInfos[i]);
        }

        cache->cache.Clear();
    }

    void GenericTypeCache_Invalidate(GenericTypeCache * cache, LongBoolMap changedFiles, TempAllocator * allocator) {
        TypeInfo** pTypeInfos = allocator->AllocateUncleared<TypeInfo*>(cache->cache.size);
        FixedPodList<TypeInfo*> typeInfos(pTypeInfos, cache->cache.size);
        cache->cache.GetValues(typeInfos.ToCheckedArray());
        for(int32 i = 0; i < typeInfos.size; i++) {
            if(changedFiles.Get(typeInfos[i]->declaringFile->fileId)){
                free(typeInfos[i]);
                typeInfos.SwapRemoveAt(i);
                i--;
            }
        }

        cache->cache.Clear();

        for(int32 i = 0; i < typeInfos.size; i++) {
            cache->cache.TryAdd(typeInfos[i]->GetFullyQualifiedTypeName(), typeInfos[i]);
        }

    }

    void ClearGenericTypeCache(GenericTypeCache * cache) {
        cache->cache.Clear();
    }

    void DestroyGenericTypeCache(GenericTypeCache * cache) {
        cache->~GenericTypeCache();
    }

    TypeInfo* MakeGenericType(GenericTypeCache * cache, TypeInfo * openType, CheckedArray<ResolvedType> typeArguments, TempAllocator * tempAllocator) {
        return cache->MakeGenericType(openType, typeArguments, tempAllocator);
    }
}