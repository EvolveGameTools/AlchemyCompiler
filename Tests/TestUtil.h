
#pragma once

#include "../Src/Allocation/LinearAllocator.h"
#include "../Src/Parsing2/Scanning.h"

using namespace Alchemy;

struct CheckedArray<FixedCharSpan> SplitIntoLines(char* str, int32 size, LinearAllocator* allocator) {
    // Count the number of lines
    int count = 1;
    for (int i = 0; i < size; ++i) {
        if (str[i] == '\n') {
            ++count;
        }
    }

    if (count == 0) {
        FixedCharSpan* line = allocator->AllocateUncleared<FixedCharSpan>(1);
        line[0] = FixedCharSpan(str, size);
        return CheckedArray<FixedCharSpan>(line, 1);
    }

    // Allocate memory for the array of structs
    FixedCharSpan* lines = allocator->AllocateUncleared<FixedCharSpan>(count);

    // Copy each line into the array
    int32 index = 0;
    char* start = str;
    char* end = strchr(start, '\n');
    while (end != nullptr) {
        int32 length = (int32) (end - start);
        if (end > start && end[-1] == '\r') {
            --length; // Exclude '\r' from the line length
        }
        lines[index] = FixedCharSpan(start, length);
        ++index;
        start = end + 1;
        end = strchr(start, '\n');
    }

    char* last = str + size;
    if (last > start && last[-1] == '\r') {
        --last; // Exclude '\r' from the last line's end
    }
    lines[index] = FixedCharSpan(start, (int32) (last - start));

    return CheckedArray<FixedCharSpan>(lines, count);
}

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

CheckedArray<FixedCharSpan> ReadLines(const char* filePath) {
    int32 length = 0;
    char* src = ReadFileIntoCString(filePath, &length);
    assert(src != nullptr && "file not found");
    return SplitIntoLines(src, length, GetThreadLocalAllocator());
}

CheckedArray<FixedCharSpan> TreeToLine(PodList<Alchemy::Compilation::SyntaxToken> & tokens, Alchemy::Compilation::SyntaxBase * syntaxBase, Alchemy::Compilation::TreePrintOptions printOptions = Alchemy::Compilation::TreePrintOptions::None) {
    Alchemy::Compilation::NodePrinter p(tokens.ToCheckedArray(), printOptions);
    p.PrintTree(syntaxBase);
    char * pBuffer = GetThreadLocalAllocator()->AllocateUncleared<char>(p.buffer.size);
    memcpy(pBuffer, p.buffer.array, p.buffer.size);
    CheckedArray<FixedCharSpan> retn = SplitIntoLines(pBuffer, p.buffer.size, GetThreadLocalAllocator());
    return retn;
}

bool CompareLines(CheckedArray<FixedCharSpan> expectedLines, CheckedArray<FixedCharSpan> actualLines) {

    if (expectedLines.size != actualLines.size) {
        return false;
    }

    for (int32 i = 0; i < actualLines.size; i++) {
        FixedCharSpan actual = actualLines[i];
        FixedCharSpan expected = expectedLines[i];
        if (actual.size != expected.size || memcmp(actual.ptr, expected.ptr, actual.size) != 0) {
            int32 x = expectedLines.size;
            int32 y = actualLines.size;
            char * c = actual.ptr;
            char * c2 = expected.ptr;
            printf("Error comparing line %d:\n Expected: %.*s\n Actual  : %.*s", i, expected.size, expected.ptr, actual.size, actual.ptr);
            return false;
        }
    }

    return true;
}

bool CompareLines(const char * filePath, CheckedArray<FixedCharSpan> actualLines) {
    CheckedArray<FixedCharSpan> expected = ReadLines(filePath);
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

void WriteTreeToFile(const char * file, PodList<Alchemy::Compilation::SyntaxToken> & tokens, Alchemy::Compilation::SyntaxBase * syntaxBase, Alchemy::Compilation::TreePrintOptions printOptions = Alchemy::Compilation::TreePrintOptions::None) {
    Alchemy::Compilation::NodePrinter p(tokens.ToCheckedArray(), printOptions);
    p.PrintTree(syntaxBase);
    WriteStringToFile(file, p.buffer.array, p.buffer.size);
}