#pragma once

namespace Alchemy::Compilation {

    enum class VisitEntryType : uint8 {

        Type,
        Method,
        Property,
        Indexer,
        Constructor,
        // constant, delegates etc?

    };

    struct VisitEntry {

        void* entry;
        VisitEntryType entryType;

        VisitEntry(VisitEntryType entryType, void* entry)
                : entryType(entryType)
                  , entry(entry) {}

    };

    enum class CodeGenOutputType {
        Type,
        MethodSignature,
        MethodImplementation
    };

    struct CodeGenOutput {

        char* src;
        int32 srcLength;
        CodeGenOutputType outputType;

        CodeGenOutput(CodeGenOutputType outputType, FixedCharSpan output)
                : outputType(outputType)
                  , src(output.ptr)
                  , srcLength(output.size) {}

        FixedCharSpan GetOutput() const {
            return FixedCharSpan(src, srcLength);
        }

    };

    struct SharedWorkerData {
        std::mutex* mtx{};
        CheckedArray<LinearAllocator*> perWorkerAllocators;
        CheckedArray<PodList<char>*> perWorkerOutputBuffers;
        CheckedArray<PodList<CodeGenOutput>*> perWorkerCodeGenOutputs;
        CheckedArray<MSISet<ResolvedType>*> perWorkerTypeSets;
    };

}