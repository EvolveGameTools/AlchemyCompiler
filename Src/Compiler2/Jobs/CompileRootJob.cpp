#include "./CompileRootJob.h"
#include "../../JobSystem/Job.h"
#include "../../Parsing3/Tokenizer.h"
#include "../../JobSystem/Worker.h"

namespace Alchemy::Compilation {

    void RunParseJobs(CheckedArray<SourceFileInfo*> fileInfos) {

        Await(Parallel::Batch(fileInfos.size, 5), ParseFilesJob(fileInfos, perThreadParsers, diagnostics));

    }

    void CompileRootJob::Execute() {

        builtInTypes = CheckedArray<TypeInfo*>(GetAllocator()->Allocate<TypeInfo*>(kBuiltInTypeCount), kBuiltInTypeCount);


    }

}