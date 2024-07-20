#pragma once

#include "../../JobSystem/JobSystem.h"

namespace Alchemy::Compilation {

    struct ScheduleIntrospectScopesJob : Jobs::IJob {

        CheckedArray<TypeInfo*> typeInfos;
        TypeResolutionMap* typeResolutionMap;

        ScheduleIntrospectScopesJob(CheckedArray<TypeInfo*> typeInfos, TypeResolutionMap* typeResolutionMap)
            : typeInfos(typeInfos)
            , typeResolutionMap(typeResolutionMap) {}

        void Execute(int32 start, int32 end) override {

            for (int32 i = start; i < end; i++) {

                TypeInfo* typeInfo = typeInfos[i];

                for (int32 m = 0; m < typeInfo->methodCount; m++) {
                    Schedule(IntrospectScopesJob(typeInfo, &typeInfo->methods[m], typeResolutionMap));
                }

            }

        }

    };

}