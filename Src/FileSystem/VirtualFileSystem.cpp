#include "./VirtualFileSystem.h"
#include "../Util/File.h"

namespace Alchemy {

    FixedCharSpan VirtualFileSystem::InternPath(std::filesystem::path& path) {
        std::string str = path.string();
        return internTable.Intern(FixedCharSpan((char*) str.c_str(), (int32) str.size()));
    }

    int32 VirtualFileSystem::LoadSourcesFromRealFileSystem(FixedCharSpan location, FixedCharSpan packageName, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output) {

        std::string directory(location.ptr, location.size);

        int32 start = output->size;

        namespace fs = std::filesystem;

        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            return 0;
        }

        for (const fs::directory_entry& entry: fs::recursive_directory_iterator(directory)) {

            if (!entry.is_regular_file()) {
                continue;
            }

            FixedCharSpan extension;

            std::string fsAbsolutePathString = fs::absolute(entry.path()).string();
            FixedCharSpan fsAbsoluteSpan(fsAbsolutePathString.c_str(), fsAbsolutePathString.size());
            FixedCharSpan fsFileExt = FindFileExtension(fsAbsoluteSpan.ptr, fsAbsoluteSpan.size);

            for (int32 i = 0; i < extensions.size; i++) {

                if (fsFileExt == extensions[i]) {
                    extension = extensions[i];
                    break;
                }

            }

            if (extension.ptr == nullptr) {
                continue;
            }

            FixedCharSpan absolutePathSpan = internTable.Intern(fsAbsoluteSpan);

            uint64 lastEditTime = std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(entry.path()).time_since_epoch()).count();

            new(output->Reserve(1)) VirtualFileInfo(
                packageName,
                absolutePathSpan,
                lastEditTime
            );

        }

        return output->size - start;

    }

    FixedCharSpan VirtualFileSystem::ReadFileText(FixedCharSpan absolutePath, Allocator allocator) {

        if(usingRealFileSystem) {
            return ReadFile(absolutePath, allocator);
        }

        NOT_IMPLEMENTED("ReadFileText with a virtual fs");

        return FixedCharSpan();
    }

    int32 VirtualFileSystem::LoadFileInfos(FixedCharSpan location, FixedCharSpan packageName, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output) {
        if(usingRealFileSystem) {
            return LoadSourcesFromRealFileSystem(location, packageName, extensions, output);
        }
        NOT_IMPLEMENTED("LoadFileInfos with a virtual fs");
        return 0;
    }

    VirtualFileInfo::VirtualFileInfo(FixedCharSpan packageName, FixedCharSpan absolutePath, uint64 lastEditTime)
        : packageName(packageName.ptr)
        , packageNameSize((int32) packageName.size)
        , absolutePath(absolutePath.ptr)
        , absolutePathSize((int32) absolutePath.size)
        , lastEditTime(lastEditTime) {}

}