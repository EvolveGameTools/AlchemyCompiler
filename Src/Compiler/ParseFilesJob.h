#pragma once

#include "../JobSystem/Worker.h"
#include "./Util.h"

namespace Alchemy::Compilation {

    struct ParseFilesJob : Alchemy::Jobs::IJob {

        CheckedArray<FileInfo*> fileInfos;
        CheckedArray<Parser*> perThreadParsers;
        Diagnostics* diagnostics;

        explicit ParseFilesJob(CheckedArray<FileInfo*> fileInfos, CheckedArray<Parser*> perThreadParsers, Diagnostics* diagnostics)
            : fileInfos(fileInfos)
            , perThreadParsers(perThreadParsers)
            , diagnostics(diagnostics) {}

        void Execute(int32 start, int32 end) override {

            Parser *parser = perThreadParsers[GetWorkerId()];

            for (int32 i = start; i < end; i++) {

                FileInfo* fileInfo = fileInfos[i];

                if (!fileInfo->wasChanged) {
                    continue;
                }

                int32 length;
                char* fileContent = ReadFileIntoCString(fileInfo->filePath.ptr, &length);
                if (!parser->TryParseFile(fileContent, length, &fileInfo->parseResult, &fileInfo->allocator)) {
                    for (int32 e = 0; e < fileInfo->parseResult.errors.size; e++) {
                        ParseError error = fileInfo->parseResult.errors[e];
                        LineColumn lineColumn = fileInfo->parseResult.GetLocationFromByteOffset(error.sourceStart);
                        diagnostics->LogErrorArgs(fileInfo, lineColumn, "Error: %.*s (%d:%d)", error.messageLength, error.message, lineColumn.line, lineColumn.column);
                    }
                }

            }

        }

    };

}