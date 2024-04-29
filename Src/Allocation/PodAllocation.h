#pragma once

#define FUNCTIONIZE(a, b)  a(b)
#define STRINGIZE(a)      #a
#define INT2STRING(i)     FUNCTIONIZE(STRINGIZE,i)

#define MALLOC_LINE_INFO __FILE__ ":" INT2STRING(__LINE__)

#if ALCHEMY_MALLOC_DEBUG != 0

    namespace MallocDebug {
        void* malloc_tracked(size_t size, const char* location);
        void* malloc_tracked_cleared(size_t size, const char* location);
        void free_tracked(void * ptr, size_t byteCount, const char * location);
        void ReportAndFreeLeakedAllocations();
    }

    #define Mallocate(size) MallocDebug::malloc_tracked_cleared(size, MALLOC_LINE_INFO)
    #define MallocateTyped(type, count) (type*)MallocDebug::malloc_tracked_cleared((sizeof(type)) * count, MALLOC_LINE_INFO)

    #define MallocateUncleared(size) MallocDebug::malloc_tracked(size, MALLOC_LINE_INFO)
    #define MallocateTypedUncleared(type, count) (type*)MallocDebug::malloc_tracked((sizeof(type)) * count, MALLOC_LINE_INFO)

    #define Mfree(ptr, size) MallocDebug::free_tracked(ptr, size, MALLOC_LINE_INFO)
    #define MfreeTyped(ptr, count) MallocDebug::free_tracked(ptr, (sizeof(*ptr) * count), MALLOC_LINE_INFO)

#else

    #include <memory>

    namespace MallocDebug {
        void ReportAndFreeLeakedAllocations();
    }

    #define Mallocate(size) calloc(1, size)
    #define MallocateTyped(type, count) (type*)calloc(count, (sizeof(type)))

    #define MallocateUncleared(size) malloc(size)
    #define MallocateTypedUncleared(type, count) (type*)malloc((sizeof(type)) * count)

    #define Mfree(ptr, size) free(ptr)
    #define MfreeTyped(ptr, count) free(ptr)

#endif