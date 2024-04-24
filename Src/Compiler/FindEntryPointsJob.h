#pragma once

#include "./EntryPoint.h"

namespace Alchemy::Compilation {

    struct FindEntryPointsJob : Jobs::IJob {

        Diagnostics* diagnostics;
        CheckedArray<FileInfo*> fileInfos;
        CheckedArray<FixedCharSpan> entryPointPatterns;
        PodList<EntryPoint>* output;
        FixedCharSpan rootPackageName;

        FindEntryPointsJob(FixedCharSpan rootPackageName, CheckedArray<FileInfo*> fileInfos, CheckedArray<FixedCharSpan> entryPointPatterns, PodList<EntryPoint>* output, Diagnostics* diagnostics)
                : fileInfos(fileInfos)
                  , output(output)
                  , rootPackageName(rootPackageName)
                  , entryPointPatterns(entryPointPatterns)
                  , diagnostics(diagnostics) {}

        struct EntryPointLookup {

            FixedCharSpan fullyQualifiedName;
            FixedCharSpan packageName;
            FixedCharSpan methodName;
            bool matchAnyExportedMethod{};

        };

        void Execute() override {

            // we need to be sure we don't get duplicates
            MSISet<EntryPoint> entryPointMap(64);

            EntryPointLookup* _lookups = GetAllocator()->AllocateUncleared<EntryPointLookup>(entryPointPatterns.size);
            CheckedArray<EntryPointLookup> lookups(_lookups, entryPointPatterns.size);

            for (int32 k = 0; k < entryPointPatterns.size; k++) {

                EntryPointLookup* lookup = &_lookups[k];
                FixedCharSpan pattern = entryPointPatterns[k];
                int32 s = pattern.size;
                char* c = pattern.ptr;

                bool foundSeparator = false;
                int32 dotIndex = pattern.size;
                int32 separatorIndex = -1;

                lookup->fullyQualifiedName = pattern;
                lookup->matchAnyExportedMethod = true;
                lookup->packageName = rootPackageName;

                for (int32 z = 0; z < s - 1; z++) {
                    if (c[z] == ':' && c[z + 1] == ':') {
                        if (!foundSeparator) {
                            separatorIndex = z;
                        }
                        foundSeparator = true;
                    }
                    if (c[z] == '.') {
                        dotIndex = z;
                        lookup->methodName = FixedCharSpan(c + z, s);
                        lookup->matchAnyExportedMethod = false;
                        break;
                    }
                }

                if (foundSeparator) {
                    lookup->fullyQualifiedName = FixedCharSpan(c, dotIndex);
                    lookup->packageName = FixedCharSpan(c, separatorIndex);
                } else {
                    int32 allocSize = rootPackageName.size + 3 + dotIndex;
                    char* name = GetAllocator()->Allocate<char>(allocSize);
                    memcpy(name, rootPackageName.ptr, rootPackageName.size);
                    name[rootPackageName.size + 0] = ':';
                    name[rootPackageName.size + 1] = ':';
                    memcpy(name + rootPackageName.size + 2, pattern.ptr, pattern.size);
                    name[allocSize - 1] = '\0';
                    lookup->fullyQualifiedName = FixedCharSpan(name, allocSize - 1);
                }
            }

            for (int32 i = 0; i < lookups.size; i++) {
                EntryPointLookup &lookup = lookups[i];

                for (int32 fileIndex = 0; fileIndex < fileInfos.size; fileIndex++) {
                    FileInfo* fileInfo = fileInfos[fileIndex];

                    if (fileInfo->packageName != lookup.packageName) {
                        continue;
                    }

                    for (int32 j = 0; j < fileInfo->declaredTypes.size; j++) {
                        TypeInfo* declaredType = fileInfo->declaredTypes[j].typeInfo;
                        TypeContext * typeContext = fileInfo->declaredTypes[j].typeContext;

                        if (lookup.fullyQualifiedName != declaredType->fullyQualifiedName) {
                            continue;
                        }

                        if ((declaredType->modifiers & DeclarationModifier::Export) == 0) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromNodeIndex(declaredType->nodeIndex.index);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Entry points must be marked as `export`. %.*s is not marked as exported but is requested as an entry point", declaredType->GetFullyQualifiedTypeName().size, declaredType->GetFullyQualifiedTypeName().ptr);
                            continue;
                        }

                        if((declaredType->flags & TypeInfoFlags::IsGeneric) != 0) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromNodeIndex(declaredType->nodeIndex.index);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Generic types cannot be used as entry points, %.*s", declaredType->GetFullyQualifiedTypeName().size, declaredType->GetFullyQualifiedTypeName().ptr);
                            continue;
                        }

                        if((declaredType->typeClass != TypeClass::Class)) {
                            LineColumn lc = fileInfo->parseResult.GetLocationFromNodeIndex(declaredType->nodeIndex.index);
                            diagnostics->LogErrorArgs(fileInfo, lc, "Only class types can be used as entry points, %.*s is not a class", declaredType->fullyQualifiedNameLength, declaredType->fullyQualifiedName);
                            continue;
                        }

                        // todo -- only 1 method per name should able to be marked export and it cannot be generic

                        if (lookup.matchAnyExportedMethod) {

                            for (int32 m = 0; m < declaredType->methodCount; m++) {
                                MethodInfo* methodInfo = &declaredType->methods[m];

                                if ((methodInfo->modifiers & DeclarationModifier::Export) == 0) {
                                    continue;
                                }

                                entryPointMap.TryAdd(EntryPoint(methodInfo));

                                break;
                            }
                        } else {
                            for (int32 m = 0; m < declaredType->methodCount; m++) {
                                MethodInfo* methodInfo = &declaredType->methods[m];
                                if (methodInfo->GetName() != lookup.methodName) {
                                    continue;
                                }

                                if ((methodInfo->modifiers & DeclarationModifier::Export) == 0) {
                                    LineColumn lc = fileInfo->parseResult.GetLocationFromNodeIndex(methodInfo->nodeIndex.index);
                                    diagnostics->LogErrorArgs(fileInfo, lc, "Method must be marked `export` in order to be used as an entry point");
                                    continue;
                                }

                                entryPointMap.TryAdd(EntryPoint(methodInfo));
                                break;
                            }
                        }
                    }
                }

            }

            output->EnsureCapacity(entryPointMap.GetSize());
            output->size = entryPointMap.GetSize();
            entryPointMap.GetKeys(output->ToCheckedArray());

        }

    };

}