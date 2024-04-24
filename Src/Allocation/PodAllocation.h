#pragma once

#include "../DLLExport.h"
#include "../PrimitiveTypes.h"

#define MallocByteArray(size, align) MallocByteArrayImpl(size, align, __FILE__, __LINE__)

#define MallocByteArrayCleared(size, align) MallocByteArrayClearedImpl(size, align, __FILE__, __LINE__)

extern "C" DLLEXPORT
void* MallocByteArrayImpl(size_t size, size_t align, const char* file, int32 line);

extern "C" DLLEXPORT
void* MallocByteArrayClearedImpl(size_t size, size_t align, const char* file, int32 line);

extern "C" DLLEXPORT
void FreeByteArray(void* array, size_t align);

extern "C" DLLEXPORT
void* MallocProtectable(size_t size);

extern "C" DLLEXPORT
void FreeProtectable(void* ptr);

extern "C" DLLEXPORT
void MakeProtectableReadOnly(void* ptr);