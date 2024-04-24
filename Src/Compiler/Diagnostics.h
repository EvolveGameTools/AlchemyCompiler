#pragma once

#include <mutex>
#include <stdarg.h>
#include <iostream>
#include "./TypeInfo.h"
#include "../Parsing/ParseResult.h"
#include "./FileInfo.h"

namespace Alchemy::Compilation {

    struct Diagnostics {

        struct Diagnostic {

            FileInfo* file;
            char* message;
            LineColumn lineColumn;
            bool free;

            Diagnostic(FileInfo* file, LineColumn lineColumn, char* message, bool free)
                : file(file)
                , message(message)
                , lineColumn(lineColumn)
                , free(free) {}

        };

        std::mutex mutex;
        PodList<Diagnostic> list;

        void LogError(FileInfo* fileInfo, LineColumn lineColumn, char* message) {
            LogErrorInternal(Diagnostic(fileInfo, lineColumn, message, false));
        }

        void LogErrorConst(FileInfo* fileInfo, LineColumn lineColumn, const char* message) {
            LogErrorInternal(Diagnostic(fileInfo, lineColumn, (char*)message, false));
        }

        void LogErrorConst(FileInfo* fileInfo, TokenRange tokenRange, const char* message) {
            LineColumn lineColumn = fileInfo->parseResult.GetLocationFromToken(tokenRange.start);
            LogErrorInternal(Diagnostic(fileInfo, lineColumn, (char*)message, false));
        }

        void LogErrorArgs(FileInfo* fileInfo, TokenRange tokenRange, const char* message, ...) {
            va_list args;
            va_start(args, message);

            int32 size = vsnprintf(nullptr, 0, message, args) + 1;
            char* pStr = (char*) MallocByteArray(size + 1, alignof(char));
            vsnprintf(pStr, size, message, args);
            pStr[size] = '\0';

            va_end(args);

            LogErrorInternal(Diagnostic(fileInfo, fileInfo->parseResult.GetLocationFromToken(tokenRange.start), pStr, true));
        }

        void LogErrorArgs(FileInfo* fileInfo, LineColumn lineColumn, const char* message, ...) {

            va_list args;
            va_start(args, message);

            int32 size = vsnprintf(nullptr, 0, message, args) + 1;
            char* pStr = (char*) MallocByteArray(size + 1, alignof(char));
            vsnprintf(pStr, size, message, args);
            pStr[size] = '\0';

            va_end(args);

            LogErrorInternal(Diagnostic(fileInfo, lineColumn, pStr, true));

        }

        void LogErrorInternal(const Diagnostic& diagnostic) {
            std::scoped_lock lock(mutex);
            list.Add(diagnostic);
            std::cout << diagnostic.file->filePath.ptr << " (" << diagnostic.lineColumn.line << ":" << diagnostic.lineColumn.column << ") " << diagnostic.message << std::endl;
        }

    };

}