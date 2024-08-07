#include <cstdio>
#include <cassert>
#include <cerrno>
#include "./File.h"

#ifndef _WIN32

errno_t fopen_s(FILE** f, const char* name, const char* mode) {
    errno_t ret = 0;
    assert(f);
    *f = fopen(name, mode);
    /* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
    if (!*f)
        ret = errno;
    return ret;
}

#endif

Alchemy::FixedCharSpan Alchemy::ReadFile(Alchemy::FixedCharSpan filePath, Alchemy::Allocator allocator) {
    FILE* file;
    char fileName[512];

    if (filePath.size > 511) {
        return FixedCharSpan();
    }

    memcpy(fileName, filePath.ptr, filePath.size);
    fileName[filePath.size] = 0;

    fopen_s(&file, fileName, "rb");
    if (file == nullptr) {
        perror("Failed to open file");
        return FixedCharSpan();
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char* buffer = allocator.AllocateUncleared<char>(fileSize + 1);

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        fclose(file);
        free(buffer);
        perror("Failed to read file");
        return FixedCharSpan();
    }

    buffer[fileSize] = '\0';

    fclose(file);
    return FixedCharSpan(buffer, fileSize);
}

char* Alchemy::ReadFileIntoCString(const char* filename, int32* length) {
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (file == nullptr) {
        perror("Failed to open file");
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(fileSize + 1);

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        fclose(file);
        free(buffer);
        perror("Failed to read file");
        return nullptr;
    }

    buffer[fileSize] = '\0';

    fclose(file);
    *length = (int32) fileSize;
    return buffer;
}
