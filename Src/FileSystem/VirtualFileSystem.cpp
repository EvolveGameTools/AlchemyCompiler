#include "./VirtualFileSystem.h"
#include "../Util/File.h"

namespace Alchemy {

    FixedCharSpan FindFileExtension(char* str, size_t size) {

        const char* dot = NULL;

        for (size_t i = size; i > 0; --i) {
            if (str[i - 1] == '.') {
                dot = &str[i - 1];
                break;
            }
        }

        // If no dot is found or it's the first character, there is no extension
        if (!dot || dot == str) {
            return FixedCharSpan();
        }
        size_t x = size - ((dot + 1) - str);
        return FixedCharSpan(dot + 1, x);
    }

    VirtualFileSystem::VirtualFileSystem(FileSystemType fileSystemType)
        : internAllocator(MEGABYTES(512), KILOBYTES(32))
        , fileSystemType(fileSystemType)
        , internTable(internAllocator.MakeAllocator(), 512) {}

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

        if (fileSystemType == FileSystemType::Real) {
            return ReadFile(absolutePath, allocator);
        }

        // todo -- don't loop stupidly
        for (int32 i = 0; i < vFileInfos.size; i++) {
            if (vFileInfos[i].info.GetAbsolutePath() == absolutePath) {
                return vFileInfos[i].content;
            }
        }

        return FixedCharSpan();
    }

    int32 VirtualFileSystem::LoadFileInfos(FixedCharSpan & packageName, FixedCharSpan & location, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output) {

        if (fileSystemType == FileSystemType::Real) {
            return LoadSourcesFromRealFileSystem(location, packageName, extensions, output);
        }

        int32 start = output->size;
        for (int32 i = 0; i < vFileInfos.size; i++) {
            VirtualFileInfo& info = vFileInfos[i].info;

            if (info.GetPackageName() != packageName) {
                continue;
            }

            if(!info.GetAbsolutePath().StartsWith(location)) {
                continue;
            }

            FixedCharSpan ext = FindFileExtension(info.absolutePath, info.absolutePathSize);

            for (int32 e = 0; e < extensions.size; e++) {
                if (extensions[e] == ext) {
                    output->Add(info);
                    break;
                }
            }

        }
        return output->size - start;
    }

    void VirtualFileSystem::AddFile(VirtualFileInfo info, FixedCharSpan contents) {
        // todo -- don't loop stupidly
        for (int32 i = 0; i < vFileInfos.size; i++) {
            if (vFileInfos[i].info.GetAbsolutePath() == info.GetAbsolutePath()) {
                vFileInfos[i].info = info;
                vFileInfos[i].content = contents;
                return;
            }
        }
        vFileInfos.Add(FileData {info, contents});
    }

    VirtualFileInfo::VirtualFileInfo(FixedCharSpan packageName, FixedCharSpan absolutePath, uint64 lastEditTime)
        : packageName(packageName.ptr)
        , packageNameSize((int32) packageName.size)
        , absolutePath(absolutePath.ptr)
        , absolutePathSize((int32) absolutePath.size)
        , lastEditTime(lastEditTime) {}

}