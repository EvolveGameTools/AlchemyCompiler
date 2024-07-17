#include <filesystem>
#include "./Compiler.h"
#include "../Util/Hash.h"
#include "../FileSystem/VirtualFileSystem.h"
#include "../Allocation/ThreadLocalTemp.h"
#include "Jobs/CompileRootJob.h"

namespace Alchemy::Compilation {

    void RecurseChangedDependants(SourceFileInfo* fileInfo) {

        if (fileInfo->dependantsVisited) {
            return;
        }

        fileInfo->dependantsVisited = true;
        fileInfo->wasChanged = true;

        for (int32 i = 0; i < fileInfo->dependants.size; i++) {
            RecurseChangedDependants(fileInfo->dependants[i]);
        }

    }

    void CreateVirtualFilesFromFileSystem(CheckedArray<AssemblyInfo> libraries, PodList<VirtualFileInfo> * list) {

        for (int32 i = 0; i < libraries.size; i++) {

            AssemblyInfo info = libraries[i];

            static std::string extension("wgt");

            std::string directory(info.path.ptr, info.path.size);

            namespace fs = std::filesystem;

            if (!fs::exists(directory) || !fs::is_directory(directory)) {
                continue;
            }

            for (const fs::directory_entry& entry: fs::recursive_directory_iterator(directory)) {

                if (!entry.is_regular_file() || entry.path().extension() != extension) {
                    continue;
                }

                fs::path absolutePath = fs::absolute(entry.path().c_str());
                std::string absolutePathStr = entry.path().string();

                VirtualFileInfo * file = list->Reserve(1);
                file->absolutePath = (char*)absolutePathStr.c_str();
                file->absolutePathSize = absolutePathStr.size();
                file->packageName = info.assemblyName.ptr;
                file->packageNameSize = info.assemblyName.size;
                file->lastEditTime = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(entry.path()).time_since_epoch()).count();

            }

        }

    }

    void Compiler::LoadDependencies() {}

    void Compiler::Compile() {

        // todo -- figure out where these are coming from

        LoadDependencies();
        packages.Add(PackageInfo()); // main source

        sourceFileBuffer.size = 0;

        for(int32 i = 0; i < packages.size; i++) {
            vfs.LoadSourcesFromRealFileSystem(packages[i].packageName, packages[i].absolutePath, &sourceFileBuffer);
        }

        SetupCompilationRun(GetThreadLocalAllocator(), sourceFileBuffer.ToCheckedArray());

        int32 changeCount = 0;

        for(int32 i = 0; i < sourceFileBuffer.size; i++) {
            if(fileInfos[i]->wasChanged) {
                changeCount++;
            }
        }

        CheckedArray<SourceFileInfo*> changedFiles(GetThreadLocalAllocator()->Allocate<SourceFileInfo*>(changeCount), changeCount);

        changeCount = 0;

        for(int32 i = 0; i < fileInfos.size; i++) {
            if(fileInfos[i]->wasChanged) {
                changedFiles[changeCount++] = fileInfos[i];
            }
        }

        jobSystem.Execute(ParseFilesJob(&vfs, fileInfos));





    }

    void Compiler::SetupCompilationRun(TempAllocator* tempAllocator, CheckedArray<VirtualFileInfo> files) {

        TempAllocator::ScopedMarker m(tempAllocator);

        // init file state
        for (int32 i = 0; i < fileInfos.size; i++) {
            SourceFileInfo* fileInfo = fileInfos.Get(i);
            fileInfo->dependants.size = 0;
            fileInfo->wasTouched = false;
            fileInfo->wasChanged = false;
            fileInfo->dependantsVisited = false;
        }

        // compute file dependants (could be done at the end of compilation instead)

        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            for (int32 d = 0; d < fileInfo->dependencies.size; d++) {
                fileInfo->dependencies.Get(d)->dependants.Add(fileInfo);
            }

        }

        // build a lookup table for our files to see if they were created this run or not
        int32 pow2Size = MathUtil::CeilPow2(fileInfos.size * 2); // *2 for a ~50 threshold, it's all temp memory anyway
        int32 exponent = MathUtil::LogPow2(pow2Size);

        SourceFileInfo** table = tempAllocator->Allocate<SourceFileInfo*>(pow2Size);

        for (int32 i = 0; i < fileInfos.size; i++) {
            SourceFileInfo* file = fileInfos.Get(i);
            int32 h = MsiHash::FNV1a(file->path.ptr, file->path.size);
            int32 idx = h;

            while (true) {
                idx = MsiHash::Lookup32(h, exponent, idx);
                if (table[idx] == nullptr) {
                    table[idx] = file;
                    break;
                }
            }

        }

        for(int32 i = 0; i < files.size; i++) {
            VirtualFileInfo * vFile = files.GetPointer(i);
            int32 h = MsiHash::FNV1a(vFile->absolutePath, vFile->absolutePathSize);
            int32 idx = h;
            while (true) {
                idx = MsiHash::Lookup32(h, exponent, idx);

                SourceFileInfo* fileInfo = table[idx];

                if (fileInfo == nullptr) {
                    // new file
                    fileInfo = new(fileAllocator.Allocate()) SourceFileInfo();
                    fileInfo->wasTouched = true;
                    fileInfo->wasChanged = true;
                    fileInfo->dependantsVisited = false;
                    fileInfo->lastEditTime = vFile->lastEditTime;
                    fileInfo->path = vFile->GetAbsolutePath();
                    fileInfo->assemblyName = vFile->GetPackageName();
                    fileInfos.Add(fileInfo);
                    break;
                }

                if (fileInfo->path == vFile->GetAbsolutePath()) {

                    // found it
                    fileInfo->wasTouched = true;

                    if(fileInfo->assemblyName != vFile->GetPackageName()) {
                        // this would change fully qualified names so we need to treat this as though it were invalidated
                        fileInfo->assemblyName = vFile->GetPackageName();
                        fileInfo->wasChanged = true;
                    }

                    if (vFile->lastEditTime != fileInfo->lastEditTime) {
                        fileInfo->lastEditTime = vFile->lastEditTime;
                        fileInfo->wasChanged = true;
                    }

                    break;
                }

                // keep looping, paths were not the same

            }
        }

        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            // if we changed a file or didn't touch a file (ie removed it) then we need to tell dependants it's gone
            if ((fileInfo->wasChanged || !fileInfo->wasTouched) && !fileInfo->dependantsVisited) {
                RecurseChangedDependants(fileInfo);
            }

        }

        // we need to remove dead files and invalidate changed ones now
        for (int32 i = 0; i < fileInfos.size; i++) {

            SourceFileInfo* fileInfo = fileInfos.Get(i);

            if (!fileInfo->wasTouched) {
                fileAllocator.Free(fileInfo);
                fileInfo->~SourceFileInfo();
                fileInfos.SwapRemoveAt(i);
                i--;
            }
            else if(fileInfo->wasChanged) {
                fileInfo->Invalidate();
            }

        }
    }


}