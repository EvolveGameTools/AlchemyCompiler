#pragma once

#include "../JobSystem/Worker.h"
#include "FileInfo.h"

namespace Alchemy::Compilation {

    struct AnalyzeTypes : Alchemy::Jobs::IJob {

        CheckedArray<DeclaredType> declaredTypes;

        explicit AnalyzeTypes(CheckedArray<DeclaredType> declaredTypes)
            : declaredTypes(declaredTypes) {}


        void Execute(int32 start, int32 end) override {

            for (int32 i = start; i < end; i++) {
                Run(declaredTypes[i]);
            }

        }

        void Run(DeclaredType declaredType) {

            // make sure we have valid declaration flags
            // make sure we have unique member names
            // make sure we have valid method definitions
            // convert optional arguments to explict methods
            FileInfo * fileInfo = declaredType.typeInfo->declaringFile;

            if(!fileInfo->wasChanged) {
                return; // no work to do
            }

            TempAllocator::ScopedMarker marker(GetAllocator());

            int32 methodCount = 0;

        }

    };

}