#include "./CompileRootJob.h"
#include <filesystem>

namespace Alchemy::Compilation {

    namespace fs = std::filesystem;

    void CompileRootJob::Execute() {

        std::string extension(".wgt");

        // these should be jobs probably but we'd need to figure out how to handle memory between threads

        PagedList<FileInfo*> addedFiles(GetAllocator(), 32);
        PagedList<FileInfo*> touchedFiles(GetAllocator(), 128);
        PagedList<FileInfo*> changedFiles(GetAllocator(), 128);

        for(int32 i = 0; i < assemblies.size; i++) {
            LoadAssemblyFiles(assemblies[i], &addedFiles, &touchedFiles, &changedFiles);
        }

        compiler->SetupCompilationRun(GetAllocator());

    }

    void CompileRootJob::LoadAssemblyFiles(AssemblyInfo info, PagedList<FileInfo*> * addedFiles, PagedList<FileInfo*>* touchedFiles, PagedList<FileInfo*>* changedFiles) {

        static std::string extension("wgt");

        std::string directory(info.path.ptr, info.path.size);

        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return;
        }

        for (const auto& entry : fs::recursive_directory_iterator(directory)) {

            if (!entry.is_regular_file() || entry.path().extension() != extension) {
                continue;
            }

            fs::path absolutePath = fs::absolute(entry.path());
            std::string absolutePathStr = entry.path().string();

            FixedCharSpan absolute((char*) absolutePathStr.c_str(), (int32) absolutePathStr.length());

            FileInfo* fileInfo = nullptr;

            if (fileInfoByAbsolutePath.TryGetValue(absolute, &fileInfo)) {
                touchedFiles->Add(fileInfo);
                uint64 lastEditTime = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(entry.path()).time_since_epoch()).count();
                if (lastEditTime != fileInfo->lastEditTime) {
                    fileInfo->lastEditTime = lastEditTime;
                    changedFiles->Add(fileInfo);
                }
            }
            else {
                addedFiles->Add(new(compiler->fileAllocator.Allocate()) FileInfo(info.assemblyName, absolute));
            }

        }

    }

}