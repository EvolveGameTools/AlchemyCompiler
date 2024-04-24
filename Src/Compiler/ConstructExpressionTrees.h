#pragma once

#include "../JobSystem/Job.h"
#include "./FileInfo.h"
#include "../Collections/MSIHashmap.h"
#include "./Diagnostics.h"
#include "./Expression.h"
#include "./ExpressionBuilder.h"
#include "./ScopeIntrospector.h"
#include "./GatherCodeGenEntries.h"
#include "EntryPoint.h"
#include "AnalyzeTypes.h"

namespace Alchemy::Compilation {

    struct IntrospectJob : Alchemy::Jobs::IJob {

        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        Diagnostics* diagnostics;
        CheckedArray<FileInfo*> fileList;
        CheckedArray<TypeInfo*> builtInTypes;
        GenericTypeCache * genericTypeCache;

        IntrospectJob(CheckedArray<FileInfo*> fileList, GenericTypeCache * genericTypeCache,  MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, CheckedArray<TypeInfo*> builtInTypes, Diagnostics* diagnostics)
            : fileList(fileList)
            , resolveMap(resolveMap)
            , builtInTypes(builtInTypes)
            , genericTypeCache(genericTypeCache)
            , diagnostics(diagnostics) {}

        void Execute(int32 start, int32 end) override {

            TempAllocator::ScopedMarker marker(GetAllocator());

            ScopeIntrospector introspector(genericTypeCache, resolveMap, builtInTypes, diagnostics, GetAllocator());

            for (int32 i = start; i < end; i++) {

                FileInfo * fileInfo = fileList[i];

                for(int32 j = 0; j < fileInfo->declaredTypes.size; j++) {
                    introspector.Introspect(fileInfo->declaredTypes[j]);
                }

            }

        }

    };


    struct ConstructExpressionTrees : Alchemy::Jobs::IJob {

        CheckedArray<FileInfo*> fileList;
        MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap;
        Diagnostics* diagnostics;
        CheckedArray<TypeInfo*> builtInTypes;
        GenericTypeCache * genericTypeCache;

        ConstructExpressionTrees(CheckedArray<FileInfo*> fileList, GenericTypeCache * genericTypeCache, MSIDictionary<FixedCharSpan, TypeInfo*>* resolveMap, CheckedArray<TypeInfo*> builtInTypes, Diagnostics* diagnostics)
            : fileList(fileList)
            , resolveMap(resolveMap)
            , diagnostics(diagnostics)
            , genericTypeCache(genericTypeCache)
            , builtInTypes(builtInTypes) {}

        void Execute() override {

            int32 cnt = 0;
            for (int32 i = 0; i < fileList.size; i++) {
                cnt += fileList[i]->declaredTypes.size;
            }

            CheckedArray<DeclaredType> typeList(GetAllocator()->Allocate<DeclaredType>(cnt), cnt);

            cnt = 0;
            for (int32 i = 0; i < fileList.size; i++) {
                FileInfo* fileInfo = fileList[i];
                CheckedArray<DeclaredType> declaredTypes = fileInfo->declaredTypes;

                for (int32 d = 0; d < declaredTypes.size; d++) {
                    typeList[cnt++] = declaredTypes[d];
                }
            };

//            Await(Jobs::Parallel::Batch(typeList.size, 3), AnalyzeTypes(typeList));

            Await(Jobs::Parallel::Batch(fileList.size, 1), IntrospectJob(fileList, genericTypeCache, resolveMap, builtInTypes, diagnostics));

        }

    };

}