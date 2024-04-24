#pragma once

#include "../PrimitiveTypes.h"

#include <algorithm>
#include <cmath>

namespace Alchemy {

    template<class T>
    void InsertionSort(T* arr, int32 low, int32 high);

    template<class T, class CompareTo>
    void InsertionSort(T* arr, int32 low, int32 high, const CompareTo &compareTo);

    template<typename T, typename CompareTo>
    void IntrospectionSort(T* array, int32 size, CompareTo compareTo);

    template<class T>
    void IntrospectionSort(T* arr, int32 size);

    namespace {

        template<class T, class CompareTo>
        int32 Partition(T* arr, int32 low, int32 high, const CompareTo &compare) {
            T pivot = arr[high];
            int32 i = low - 1;
            for (int32 j = low; j <= high - 1; j++) {
                if (compare(arr[j], pivot) <= 0) {
                    i++;
                    std::swap(arr[i], arr[j]);
                }
            }
            std::swap(arr[i + 1], arr[high]);
            return (i + 1);
        }

        template<class T>
        int32 Partition(T* arr, int32 low, int32 high) {
            T pivot = arr[high];
            int32 i = low - 1;
            for (int32 j = low; j <= high - 1; j++) {
                if (arr[j].CompareTo(pivot) <= 0) {
                    i++;
                    std::swap(arr[i], arr[j]);
                }
            }
            std::swap(arr[i + 1], arr[high]);
            return (i + 1);
        }

        template<class T>
        void IntrospectionSortHelper(T* arr, int32 low, int32 high, int32 depthLimit) {
            while (low < high) {
                if (depthLimit == 0) {
                    InsertionSort(arr, low, high);
                    return;
                }
                else {
                    int32 pivot = Partition(arr, low, high);
                    IntrospectionSortHelper(arr, low, pivot - 1, depthLimit - 1);
                    low = pivot + 1;
                    depthLimit--;
                }
            }
        }

        template<class T, class CompareTo>
        void IntrospectionSortHelper(T* arr, int32 low, int32 high, int32 depthLimit, CompareTo compareTo) {
            while (low < high) {
                if (depthLimit == 0) {
                    InsertionSort(arr, low, high, compareTo);
                    return;
                }
                else {
                    int32 pivot = Partition(arr, low, high, compareTo);
                    IntrospectionSortHelper(arr, low, pivot - 1, depthLimit - 1, compareTo);
                    low = pivot + 1;
                    depthLimit--;
                }
            }
        }
    }

    template<class T>
    void InsertionSort(T* arr, int32 low, int32 high) {
        for (int32 i = low + 1; i <= high; i++) {
            T key = arr[i];
            int32 j = i - 1;
            while (j >= low && arr[j].CompareTo(key) > 0) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    template<class T, class CompareTo>
    void InsertionSort(T* arr, int32 low, int32 high, const CompareTo &compareTo) {
        for (int32 i = low + 1; i <= high; i++) {
            T &key = arr[i];
            int32 j = i - 1;
            while (j >= low && compareTo(arr[j], key) > 0) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    template<class T, class CompareTo>
    void InsertionSort(T* arr, int32 size, const CompareTo &compareTo) {
        for (int32 i = 1; i < size; i++) {
            T key = arr[i];
            int32 j = i - 1;
            while (j >= 0 && compareTo(arr[j], key) > 0) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    template<class T>
    void IntrospectionSort(T* arr, int32 size) {
        if (size <= 1) return;
        int32 depthLimit = 2 * log2(size);
        IntrospectionSortHelper(arr, 0, size - 1, depthLimit);
    }

    template<typename T, typename CompareTo>
    void IntrospectionSort(T* array, int32 size, CompareTo compareTo) {
        if (size <= 1) return;
        int32 depthLimit = 2 * log2(size);
        IntrospectionSortHelper(array, 0, size - 1, depthLimit, compareTo);
    }
}

