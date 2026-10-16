#include "msort.h"

#include <algorithm>
#include <cstddef>
#include <memory>

namespace {

// Serial insertion sort without recursion, used for runs shorter than the threshold.
void insertion_sort(int* arr, std::size_t n) {
    for (std::size_t i = 1; i < n; ++i) {
        const int value = arr[i];
        std::size_t j = i;
        while (j > 0 && arr[j - 1] > value) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = value;
    }
}

// Merges the sorted runs arr[0, mid) and arr[mid, n) into arr[0, n). The left
// run is copied to buffer[0, mid) first; writing back into arr never overtakes
// the unread part of the right run, so no other scratch space is needed.
void merge_halves(int* arr, int* buffer, std::size_t mid, std::size_t n) {
    std::copy(arr, arr + mid, buffer);
    std::size_t left = 0;
    std::size_t right = mid;
    std::size_t out = 0;
    while (left < mid && right < n) {
        arr[out++] = (arr[right] < buffer[left]) ? arr[right++] : buffer[left++];
    }
    // Whatever is left of the right run is already in its final place.
    std::copy(buffer + left, buffer + mid, arr + out);
}

// Sorts arr[0, n), using buffer[0, n) as scratch space. As in the fork/join
// pseudocode for parallel merge sort, the left half is sorted in a new OpenMP
// task while the current task sorts the right half; runs shorter than the
// threshold are sorted serially to avoid the task overhead.
void merge_sort(int* arr, int* buffer, std::size_t n, std::size_t threshold) {
    if (n < threshold || n < 2) {
        insertion_sort(arr, n);
        return;
    }
    const std::size_t mid = n / 2;
#pragma omp task
    merge_sort(arr, buffer, mid, threshold);
    merge_sort(arr + mid, buffer + mid, n - mid, threshold);
#pragma omp taskwait
    merge_halves(arr, buffer, mid, n);
}

}  // namespace

void msort(int* arr, const std::size_t n, const std::size_t threshold) {
    std::unique_ptr<int[]> buffer(new int[n]);
    // One thread starts the recursion; the tasks it creates are executed by
    // all the threads of the team.
#pragma omp parallel
#pragma omp single
    merge_sort(arr, buffer.get(), n, threshold);
}
