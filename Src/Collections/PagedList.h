#pragma once

#include "../Allocation/LinearAllocator.h"

namespace Alchemy {

    template<typename T>
    struct PagedList {

        struct Page {
            Page* next {};
            T* data {};
            int32 size {};
        };

        Page firstPage;
        Page* currentPage;
        int32 pageCapacity;
        int32 size;
        LinearAllocator* allocator;

        PagedList(LinearAllocator* allocator, int32 pageCapacity)
            : firstPage()
            , currentPage(nullptr)
            , allocator(allocator)
            , pageCapacity(pageCapacity)
            , size(0) {
            firstPage.data = allocator->Allocate<T>(pageCapacity);
            firstPage.size = 0;
            firstPage.next = nullptr;
            currentPage = &firstPage;
        }

        void Add(T item) {
            if (currentPage->size == pageCapacity) {
                Page* newPage = allocator->Allocate<Page>(1);
                newPage->size = 0;
                newPage->next = nullptr;
                newPage->data = allocator->AllocateUncleared<T>(pageCapacity);
                currentPage->next = newPage;
                currentPage = newPage;
            }
            currentPage->data[currentPage->size++] = item;
            size++;
        }

    };


}