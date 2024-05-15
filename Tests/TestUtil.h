
#pragma once

#include "../Src/Allocation/LinearAllocator.h"
#include "../Src/Parsing2/Scanning.h"

using namespace Alchemy;

struct Line {
    char* line;
    int32_t length;
};

struct CheckedArray<Line> SplitIntoLines(char* str, int32 size, LinearAllocator* allocator) {
    // Count the number of lines
    int count = 1;
    for (int i = 0; i < size; ++i) {
        if (str[i] == '\n') {
            ++count;
        }
    }

    if (count == 0) {
        Line* line = allocator->AllocateUncleared<Line>(1);
        line->line = str;
        return CheckedArray<Line>(line, 1);
    }

    // Allocate memory for the array of structs
    struct Line* lines = allocator->AllocateUncleared<Line>(count);

    // Copy each line into the array
    int32 index = 0;
    char* start = str;
    char* end = strchr(start, '\n');
    while (end != nullptr) {
        int32 length = (int32) (end - start);
        lines[index].line = start;
        lines[index].length = length;
        ++index;
        start = end + 1;
        end = strchr(start, '\n');
    }

    char* last = str + size;
    lines[index].line = start;
    lines[index].length = (int32) (last - start);

    return CheckedArray<Line>(lines, count);
}

#pragma once
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

char* ReadFileIntoCString(const char* filename, int32* length) {
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

CheckedArray<Line> ReadLines(const char* filePath) {
    int32 length = 0;
    char* src = ReadFileIntoCString(filePath, &length);
    assert(src != nullptr && "file not found");
    return SplitIntoLines(src, length, GetThreadLocalAllocator());
}

CheckedArray<Line> TreeToLine(PodList<Alchemy::Compilation::SyntaxToken> & tokens, Alchemy::Compilation::SyntaxBase * syntaxBase) {
    Alchemy::Compilation::NodePrinter p(tokens.ToCheckedArray());
    p.PrintTree(syntaxBase);
    return SplitIntoLines(p.buffer.array, p.buffer.size, GetThreadLocalAllocator());
}

bool CompareLines(CheckedArray<Line> expectedLines, CheckedArray<Line> actualLines) {

    if (expectedLines.size != actualLines.size) {
        return false;
    }

    for (int32 i = 0; i < actualLines.size; i++) {
        Line actual = actualLines[i];
        Line expected = expectedLines[i];
        if (actual.length != expected.length || memcmp(actual.line, expected.line, actual.length) != 0) {
            printf("Error comparing line %d:\n Expected: %.*s\n Actual  : %.*s", i, expected.length, expected.line, actual.length, actual.line);
            return false;
        }
    }

    return true;
}

bool CompareLines(const char * filePath, CheckedArray<Line> actualLines) {
    CheckedArray<Line> expected = ReadLines(filePath);
    return CompareLines(expected, actualLines);
}

bool WriteStringToFile(const char* fileName, char* content, size_t length) {
    std::ofstream file(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return false;
    }

    file.write(content, length);
    file.close();
    return true;
}