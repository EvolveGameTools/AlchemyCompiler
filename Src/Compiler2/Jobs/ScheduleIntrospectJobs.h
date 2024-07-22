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

                if((typeInfo->flags & TypeInfoFlags::InstantiatedGeneric) != 0) {
                    // we only want to directly process types that are concrete or generic templates
                    // for actual generic methods we'll process them as we encounter them
                    // we do this so we:
                    // a. don't process a bunch of unused generic methods
                    // b. allocate concrete methods ephemerally
                    // c. don't duplicate a ton of diagnostic errors
                    continue;
                }

                for (int32 m = 0; m < typeInfo->methodCount; m++) {
                    Schedule(Jobs::Parallel::Foreach(5), IntrospectScopesJob(typeInfo, &typeInfo->methods[m], typeResolutionMap));
                }

            }

        }

    };

}