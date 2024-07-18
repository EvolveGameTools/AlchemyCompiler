#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Allocation/LinearAllocator.h"
#include "../Collections/PodList.h"
#include "../Util/StringTable.h"
#include <filesystem>
#include <mutex>

namespace Alchemy {

    enum class FileSystemType {
        Real,
        Virtual
    };

    struct VirtualFileInfo {

        char* packageName;
        char* absolutePath;
        uint64 lastEditTime;
        int32 absolutePathSize;
        int32 packageNameSize;

        VirtualFileInfo(FixedCharSpan packageName, FixedCharSpan absolutePath, uint64 lastEditTime = 0);

        FixedCharSpan GetPackageName() {
            return FixedCharSpan(packageName, packageNameSize);
        }

        FixedCharSpan GetAbsolutePath() {
            return FixedCharSpan(absolutePath, absolutePathSize);
        }

    };

    struct VirtualFileSystem {

        struct FileData {
            VirtualFileInfo info;
            FixedCharSpan content;
        };

        LinearAllocator internAllocator;
        StringTable internTable;
        FileSystemType fileSystemType;

        PodList<FileData> vFileInfos;

        explicit VirtualFileSystem(FileSystemType fileSystemType);

        FixedCharSpan ReadFileText(FixedCharSpan absolutePath, Allocator allocator);

        int32 LoadSourcesFromRealFileSystem(FixedCharSpan location, FixedCharSpan packageName, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output);

        int32 LoadFileInfos(FixedCharSpan & packageName, FixedCharSpan & location, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output);

        FixedCharSpan InternPath(std::filesystem::path& path);

        void AddFile(VirtualFileInfo info, FixedCharSpan contents);

    };

}
