#pragma once

#include "../PrimitiveTypes.h"
#include "../Util/FixedCharSpan.h"
#include "../Allocation/LinearAllocator.h"
#include "../Collections/PodList.h"
#include "../Util/StringTable.h"
#include <filesystem>
#include <mutex>

namespace Alchemy {

    struct VirtualFileContents {

        uint8* content;
        size_t contentSize;

        // todo -- more shit around nuking/caching file contents

    };

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

        return FixedCharSpan(dot + 1, (dot + 1) - str);
    }

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

        StringTable internTable;
        bool usingRealFileSystem;

        FixedCharSpan ReadFileText(FixedCharSpan absolutePath, Allocator allocator);

        int32 LoadSourcesFromRealFileSystem(FixedCharSpan location, FixedCharSpan packageName, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output);

        int32 LoadFileInfos(FixedCharSpan location, FixedCharSpan packageName, CheckedArray<FixedCharSpan> extensions, PodList<VirtualFileInfo>* output);

        FixedCharSpan InternPath(std::filesystem::path& path);

    };

}
