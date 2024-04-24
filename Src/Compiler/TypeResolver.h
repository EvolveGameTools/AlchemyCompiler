#pragma once

namespace Alchemy::Compilation {

    struct TypeResolver {

        FileInfo* fileInfo;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        Diagnostics* diagnostics;
        CheckedArray<TypeInfo*> builtInTypeInfos;
        CheckedArray<TypeInfo*> inputGenericArguments;
        TempAllocator* tempAllocator;
        GenericTypeCache * genericTypeCache;

        TypeResolver() = default;

        TypeResolver(FileInfo* fileInfo, GenericTypeCache * genericTypeCache,  MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, Diagnostics* diagnostics, CheckedArray<TypeInfo*> builtInTypeInfos, CheckedArray<TypeInfo*> inputGenericArguments, TempAllocator * tempAllocator)
            : fileInfo(fileInfo)
            , resolveMap(resolveMap)
            , diagnostics(diagnostics)
            , builtInTypeInfos(builtInTypeInfos)
            , inputGenericArguments(inputGenericArguments)
            , genericTypeCache(genericTypeCache)
            , tempAllocator(tempAllocator)
            {}

        static FixedCharSpan MakeFullyQualifiedName(FixedCharSpan namespaceName, FixedCharSpan name, int32 genericCount, LinearAllocator * allocator) {

            int32 size = namespaceName.size + name.size + 2;

            if (genericCount > 0) {
                size += 1 + IntToAsciiCount(genericCount);
            }

            char* buffer = allocator->Allocate<char>(size);
            char* ptr = buffer;

            memcpy(ptr, namespaceName.ptr, namespaceName.size);
            ptr += namespaceName.size;
            ptr[0] = ':';
            ptr[1] = ':';
            ptr += 2;
            memcpy(ptr, name.ptr, name.size);
            ptr += name.size;
            if (genericCount > 0) {
                ptr[0] = '`';
                ptr++;
                IntToAscii(genericCount, ptr);
            }

            return FixedCharSpan(buffer, size);

        }


        template<class T>
        T* GetNode(NodeIndex<T> nodeIndex) {
            if (nodeIndex.index == 0) {
                return nullptr;
            }
            return (T*) fileInfo->parseResult.psiNodes.GetPointer(nodeIndex.index);
        }

        bool TryResolveTypePath(TypePathNode* typePathNode, ResolvedType* resolvedType) {

            *resolvedType = ResolvedType();

            if (typePathNode == nullptr) {
                return false;
            }

            if (typePathNode->builtInTypeName != BuiltInTypeName::Invalid) {
                if (typePathNode->builtInTypeName == BuiltInTypeName::Void) {
                    // probably bad
                    resolvedType->flags |= ResolvedTypeFlags::IsVoid;
                }
                else {
                    resolvedType->typeInfo = builtInTypeInfos[(int32) typePathNode->builtInTypeName];
                    resolvedType->builtInTypeName = typePathNode->builtInTypeName;
                }

                return true;
            }

            IdentifierNode* identifierNode = GetNode(typePathNode->namePath);

            if (identifierNode == nullptr) {
                LineColumn lineColumn = fileInfo->parseResult.GetLocationFromToken(typePathNode->tokenStart);
                FixedCharSpan src = fileInfo->GetSourceRange(typePathNode->tokenStart, typePathNode->tokenEnd);
                diagnostics->LogErrorArgs(fileInfo, lineColumn, "Unable to resolve type `%.*s`", src.size, src.ptr);
                return false;
            }

            TempAllocator::ScopedMarker scopedMarker(tempAllocator);

            char* c = tempAllocator->AllocateUncleared<char>(2048); // todo -- maybe just count?
            char* ptr = c;

            // we don't support nested generic types (and maybe generally no nested types)
            TypeArgumentListNode* generics = GetNode(typePathNode->typeArguments);
            ArrayRankNode* arrayRankNode = GetNode(typePathNode->arrayRank);
            int32 genericCount = 0;

            while (true) {

                FixedCharSpan identifier = identifierNode->GetIdentifier();

                memcpy(ptr, identifier.ptr, identifier.size);
                ptr += identifier.size;

                identifierNode = GetNode(identifierNode->next);

                if (identifierNode != nullptr) {
                    ptr[0] = ':';
                    ptr[1] = ':';
                    ptr += 2;
                }
                else {
                    if (generics != nullptr) {
                        TypePathNode* first = GetNode(generics->first);
                        TypePathNode* typePtr = first;

                        while (typePtr != nullptr) {
                            genericCount++;
                            typePtr = GetNode(typePtr->next);
                        }

                        ptr[0] = '`';
                        ptr++;
                        ptr += IntToAscii(genericCount, ptr);

                    }
                    break; // we don't support nested generics right now, maybe we change this
                }

            }

            ResolvedTypeFlags flags = ResolvedTypeFlags::None;
            if (typePathNode->isNullable) {
                flags |= ResolvedTypeFlags::IsNullable;
            }

            if (arrayRankNode != nullptr) {
                flags |= ResolvedTypeFlags::IsArray;
                if (arrayRankNode->isNullable) {
                    flags |= ResolvedTypeFlags::IsNullableArray;
                }
            }

            FixedCharSpan typeName(c, (int32) (ptr - c));
            bool found = false;

            resolvedType->flags = flags;
            resolvedType->arrayRank = arrayRankNode != nullptr ? 1 : 0; // we only support 1 array rank

            for (int32 k = 0; k < inputGenericArguments.size; k++) {
                TypeInfo* genericArgument = inputGenericArguments[k];

                if (typeName != genericArgument->typeName) {
                    continue;
                }

                found = true;
                resolvedType->typeInfo = genericArgument;

                break;
            }

            if (!found) {

                for (int32 i = 0; i < fileInfo->usingAliases.size; i++) {

                    if (fileInfo->usingAliases[i].name == typeName) {
                        resolvedType->typeInfo = fileInfo->usingAliases[i].typeInfo;
                        found = true;
                        break;
                    }

                }

                if (!found) {
                    for (int32 i = 0; i < fileInfo->usingNamespaces.size; i++) {
                        FixedCharSpan attempt = MakeFullyQualifiedName(fileInfo->usingNamespaces[i], typeName, 0, tempAllocator);
                        TypeInfo* typeInfo = nullptr;

                        if (!resolveMap->TryGetValue(attempt, &typeInfo)) {
                            continue;
                        }

                        if (found) {
                            LineColumn x = fileInfo->parseResult.GetLocationFromToken(typePathNode->tokenStart);
                            diagnostics->LogErrorArgs(fileInfo, x, "Ambiguous type match %.*s", attempt.size, attempt.ptr);
                        }
                        else {
                            found = true;
                            resolvedType->typeInfo = typeInfo;

                            if (typeInfo->IsEnum()) {
                                resolvedType->flags |= ResolvedTypeFlags::IsEnum;
                            }
                        }

                    }
                }
            }

            if (!found) {
                LineColumn lineColumn = fileInfo->parseResult.GetLocationFromToken(typePathNode->tokenStart);
                FixedCharSpan src = fileInfo->GetSourceRange(typePathNode->tokenStart, typePathNode->tokenEnd);
                diagnostics->LogErrorArgs(fileInfo, lineColumn, "Unable to resolve type `%.*s`", src.size, src.ptr);
                return false;
            }

            if (genericCount > 0) {
                CheckedArray<ResolvedType> resolvedTypes(tempAllocator->AllocateUncleared<ResolvedType>(genericCount), genericCount);

                TypePathNode* first = GetNode(generics->first);
                TypePathNode* typePtr = first;
                int32 providedGenericCount = 0;

                while (typePtr != nullptr) {
                    if (!TryResolveTypePath(typePtr, &resolvedType[providedGenericCount])) {
                        // already issued an error
                        return false;
                    }
                    providedGenericCount++;
                    typePtr = GetNode(typePtr->next);
                }

                if (genericCount != providedGenericCount) {
                    // diagnostics->LogErrorArgs();
                    return false;
                }

               resolvedType->typeInfo = MakeGenericType(genericTypeCache, resolvedType->typeInfo, resolvedTypes, tempAllocator);

            }

            if (resolvedType->typeInfo != nullptr) {
                fileInfo->AddFileReference(resolvedType->typeInfo->declaringFile);
            }

            return true;

        }

    };
}