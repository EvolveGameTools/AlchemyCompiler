#pragma once

#include "../../JobSystem/JobSystem.h"
#include "../Introspector.h"

namespace Alchemy::Compilation {

    struct IntrospectionJob : Jobs::IJob {

        CheckedArray<SourceFileInfo*> fileInfos;
        TypeResolutionMap* typeResolutionMap;

        IntrospectionJob(CheckedArray<SourceFileInfo*> fileInfos, TypeResolutionMap* typeResolutionMap)
            : fileInfos(fileInfos)
            , typeResolutionMap(typeResolutionMap) {}

        void Execute(int32 index) override {

            Introspector introspector(fileInfos[index], typeResolutionMap);

//            introspector.Introspect();

        }

    };

}