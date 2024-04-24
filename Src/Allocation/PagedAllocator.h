#pragma once

#include "../PrimitiveTypes.h"
#include "../Collections/PodList.h"

namespace Alchemy {

    template<class T>
    struct PagedAllocator {

        explicit PagedAllocator(int32 basePageSize)
            : pages(8)
            , currentPageAlloc(0)
            , baseItemsPerPage(basePageSize)
        {
            T* alloc = (T*) MallocByteArray(sizeof(T) * basePageSize, alignof(T));
            memset((void*) alloc, 0, sizeof(T) * basePageSize);
            pages.Add(Page(alloc, basePageSize, 0));
        }

        ~PagedAllocator() {
            for (int32 i = 0; i < pages.size; i++) {
                FreeByteArray(pages[i].buffer, alignof(T));
            }
        };

    private:
        struct Page {
            T* buffer;
            int32 size;
            int32 capacity;

            Page(T* buffer, int32 capacity, int32 size)
                    : buffer(buffer)
                      , capacity(capacity)
                      , size(size) {}

        };

        int32 currentPageAlloc;
        int32 baseItemsPerPage;
        PodList<Page> pages;

        int32 FindFittingPage(int32 count) {
            for (int32 i = 0; i < pages.size; i++) {
                Page &page = pages.Get(i);
                if (page.capacity - page.size >= count) {
                    return i;
                }
            }
            return -1;
        }

        void UpdateCurrentPage() {
            int32 maxCount = 0;
            for (int32 i = 0; i < pages.size; i++) {
                Page &page = pages.Get(i);
                int32 remaining = page.capacity - page.size;
                if (remaining > maxCount) {
                    maxCount = remaining;
                    currentPageAlloc = i;
                }
            }
        }

    public:
        T* AllocateUncleared(int32 count) {
            // todo keep current page as a pointer so we don't dereference pages in the common case.
            // better yet, just keep capacity & size and update them on the page if we don't hit the common case
            Page &currentPage = pages.Get(currentPageAlloc);
            int32 remaining = currentPage.capacity - currentPage.size;

            if (remaining > count) {
                T* retn = currentPage.buffer + currentPage.size;
                currentPage.size += count;
                return retn;
            }

            int32 pageIndex = FindFittingPage(count);

            if (pageIndex == -1) {
                int32 pageSize = baseItemsPerPage > count ? baseItemsPerPage : count;
                T* alloc = (T*) MallocByteArray(sizeof(T) * pageSize, alignof(T));
                pages.Add(Page(alloc, pageSize, count));
                UpdateCurrentPage();
                return alloc;
            }

            Page &page = pages.Get(pageIndex);
            T* retn = page.buffer + page.size;
            page.size += count;
            return retn;

        }

        T* Allocate(int32 count) {

            Page &currentPage = pages.Get(currentPageAlloc);
            int32 remaining = currentPage.capacity - currentPage.size;

            if (remaining > count) {
                T* retn = currentPage.buffer + currentPage.size;
                currentPage.size += count;
                return retn;
            }

            int32 pageIndex = FindFittingPage(count);

            if (pageIndex == -1) {
                int32 pageSize = baseItemsPerPage > count ? baseItemsPerPage : count;
                T* alloc = (T*) MallocByteArray(sizeof(T) * pageSize, alignof(T));
                memset((void*) alloc, 0, sizeof(T) * pageSize);
                pages.Add(Page(alloc, pageSize, count));
                UpdateCurrentPage();
                return alloc;
            }

            Page &page = pages.Get(pageIndex);
            T* retn = page.buffer + page.size;
            page.size += count;
            return retn;

        }

        void Clear() {
            currentPageAlloc = 0;

            for (int32 i = 0; i < pages.size; i++) {
                pages[i].size = 0;
            }

        }

    };

}