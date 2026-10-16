#include "matmul.h"

#include <cstddef>

// Parallel version of mmul2 from HW02 (loop order i, k, j). The rows of C are
// divided among the OpenMP threads, so each thread zeroes and accumulates only
// its own rows of C and no synchronization is needed inside the loop.
void mmul(const float* A, const float* B, float* C, const std::size_t n) {
#pragma omp parallel for
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            C[i * n + j] = 0.0f;
        }
        for (std::size_t k = 0; k < n; ++k) {
            for (std::size_t j = 0; j < n; ++j) {
                C[i * n + j] += A[i * n + k] * B[k * n + j];
            }
        }
    }
}
