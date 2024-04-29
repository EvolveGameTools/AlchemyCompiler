#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "./Panic.h"
#include "PrimitiveTypes.h"
#include "./Util/StringUtil.h"
#include <cpptrace/cpptrace.hpp>

#if ALCHEMY_DEBUG
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <sstream>

#endif

#ifdef _APPLE_
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFUserNotification.h>
#endif
#endif

namespace Alchemy {

    char messageBuffer[1024 * 2];

    void Message(const char* message) {
        #ifdef _WIN32
        #ifdef ALCHEMY_DEBUG
        int32 result = MessageBoxA(NULL, message, "Panic! Want to debug?", MB_YESNO | MB_ICONERROR);
        if (result == IDYES) {
            if (IsDebuggerPresent()) {
                DebugBreak();
            }
        }
        #endif
        #endif

        // todo -- if we have logs we'll want to log this probably
    }

    char* truncateString(char* str, const char* substring) {
        const char* pos = strstr(str, substring);
        return pos != nullptr ? const_cast<char*>(pos) : str;
    }

    int32 print_frame(char* buffer, int32 size, std::size_t counter, const cpptrace::stacktrace_frame& frame) {

        const char* format = "#%d   0x%p in %s at %s:%s%s\n";

        const char* frameAddress = "         (inlined)";
        const char* line = "(unknown line)";
        const char* col = "";
        const char* fileName = "(unknown file)";
        char hexBuffer[18] {0};
        char lineBuffer[11] {0};
        char colBuffer[11] {0};

        if (!frame.filename.empty()) {
            const char* pos = strstr(frame.filename.c_str(), "Src");
            fileName = pos != nullptr ? pos : frame.filename.c_str();
        }

        if (!frame.is_inline) {
            snprintf(hexBuffer, 18, "%p", frame.raw_address);
            frameAddress = hexBuffer;
        }

        if (frame.line.has_value()) {
            snprintf(lineBuffer, 11, "%d", frame.line.value());
            line = lineBuffer;
        }

        if (frame.column.has_value()) {
            snprintf(colBuffer, 11, ":%d", frame.column.value());
            col = colBuffer;
        }

        int32 lineSize = snprintf(buffer, size, format,
            counter,
            frameAddress,
            frame.symbol.empty() ? "(unknown symbol)" : frame.symbol.c_str(),
            fileName,
            line,
            col
        );

        return lineSize;

    }

    void Panic(PanicType panicType, void* payload) {
        memset(messageBuffer, 0, sizeof(messageBuffer));

        cpptrace::stacktrace trace = cpptrace::generate_trace();

        int32 size = 1;

        for (int32 i = 1; i < trace.frames.size(); i++) {
            size += print_frame(nullptr, 0, i, trace.frames[i]);
        }

        char* stackTrace = (char*) malloc(size);

        int32 offset = 0;
        for (int32 i = 1; i < trace.frames.size(); i++) {
            offset += print_frame(stackTrace + offset, size - offset, i, trace.frames[i]);
        }

        switch (panicType) {
            case PanicType::MemoryCorruption: {
                const char* allocationLocation = truncateString(((char**) payload)[0], "Src");
                const char* freeLocation = truncateString(((char**) payload)[1], "Src");
                snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Memory corruption at:\n    %s\nfree'd at\n    %s\n\n%s", allocationLocation, freeLocation, stackTrace);
                Message(messageBuffer);
                break;
            }
            case PanicType::InvalidFree: {
                Message("Invalid Free");
                break;
            }
            case PanicType::PartialFree: {
                uint64 allocatedBytes = ((uint64*) payload)[0];
                uint64 freedBytes = ((uint64*) payload)[1];
                char* freeLocation = ((char**) payload)[2];
                Message("Partial Free");
                break;
            }
            case PanicType::IndexOutOfBounds: {
                Message("Index out of bounds");
                break;
            }

            case PanicType::NotSupported:
            case PanicType::NotImplemented: {
                Message((char*)payload);
                break;
            }

        }

        free(stackTrace);
        // this is maybe a long jump instead
        abort();

    }
}