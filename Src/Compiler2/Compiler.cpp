#include "./Compiler.h"
#include <filesystem>
namespace Alchemy::Compilation {

    namespace fs = std::filesystem;

    void Compiler::LoadFilesInAssembly(PagedList<FileInfo*> * addedFiles, FixedCharSpan packageName, const fs::path &directory, FixedCharSpan extension) {
        for (const auto &entry: fs::directory_iterator(directory)) {
            if (fs::is_directory(entry)) {
                LoadFilesInAssembly(packageName, entry.path(), extension);
            }
            else if (FixedCharSpan(entry.path().extension().c_str()) == extension) {
                fs::path absolutePath = fs::absolute(entry.path());
                std::string absolutePathStr = entry.path().string();

                FixedCharSpan absolute((char*) absolutePathStr.c_str(), (int32) absolutePathStr.length());

                FileInfo* fileInfo = nullptr;

                if (fileInfoByAbsolutePath.TryGetValue(absolute, &fileInfo)) {
                    touchedFiles.Add(fileInfo);
                    uint64 lastEditTime = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(entry.path()).time_since_epoch()).count();
                    if (lastEditTime != fileInfo->lastEditTime) {
                        fileInfo->lastEditTime = lastEditTime;
                        changedFiles.Add(fileInfo);
                    }
                }
                else {
                    addedFiles->Add(new(fileAllocator.Allocate()) FileInfo(packageName, absolute));
                }

            }
        }
    }

    void Compiler::SetupCompilationRun(TempAllocator* tempAllocator) {

        TempAllocator::ScopedMarker m(tempAllocator);
        int32 max = fileInfos.size > touchedFiles.size ? fileInfos.size : touchedFiles.size;

        int32 mapSize = LongBoolMap::GetMapSize(max);

        LongBoolMap touchedMap(linearAllocator.Allocate<uint64>(mapSize), mapSize);
        LongBoolMap changedMap(linearAllocator.Allocate<uint64>(mapSize), mapSize);

        for (int32 i = 0; i < touchedFiles.size; i++) {
            touchedMap.Set(touchedFiles[i]->fileId);
        }

        for (int32 i = 0; i < changedFiles.size; i++) {
            changedMap.Set(changedFiles[i]->fileId);
            changedFiles[i]->wasChanged = true;
        }

        for (int32 i = 0; i < fileInfos.size; i++) {

            if (!touchedMap.Get(fileInfos[i]->fileId)) {
                changedMap.Set(fileInfos[i]->fileId);
                fileIdFreeList.Push(fileInfos[i]->fileId);

                fileInfos[i]->Dispose();
                fileAllocator.Free(fileInfos[i]);
            }

        }

        // go through the files that survived the reload (ie not moved / deleted)
        // if any dependencies were invalidated, we need to mark that file as also
        // being invalid. We loop until nothing was invalidated
        fileInfos.size = 0;
        fileInfos.AddRange(touchedFiles.array, touchedFiles.size);

        while (true) {
            bool invalidated = false;

            for (int32 i = 0; i < fileInfos.size; i++) {
                LongBoolMap dependencies = fileInfos[i]->GetDependencyMap();

                if (dependencies.ContainsAny(changedMap)) {
                    changedMap.Set(fileInfos[i]->fileId);
                    fileInfos[i]->Invalidate();
                    invalidated = true;
                }
            }

            if (!invalidated) {
                break;
            }

        }

        GenericTypeCache_Invalidate(&genericTypeCache, changedMap, tempAllocator);

        // now add the new files
        fileInfos.AddRange(addedFiles.array, addedFiles.size);
        for (int32 i = 0; i < addedFiles.size; i++) {
            if (fileIdFreeList.size > 0) {
                addedFiles[i]->fileId = fileIdFreeList.Pop();
            }
            else {
                addedFiles[i]->fileId = fileIdAllocator++;
            }
        }

        fileInfoByAbsolutePath.Clear();
        resolveMap.Clear();

        // update the internal file count maps as needed
        for (int32 i = 0; i < fileInfos.size; i++) {
            fileInfos[i]->EnsureFileCount(fileInfos.size);
        }

        for (int32 i = 0; i < fileInfos.size; i++) {
            fileInfoByAbsolutePath.TryAdd(fileInfos[i]->filePath, fileInfos[i]);
        }

        changedFiles.size = 0;
        addedFiles.size = 0;
        touchedFiles.size = 0;
    }


}