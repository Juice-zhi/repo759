#include "scan.h"

void scan(const float* arr, float* output, std::size_t n) {
    // Inclusive scan: output[i] = arr[0] + arr[1] + ... + arr[i].
    float sum = 0.0f;
    for (std::size_t i = 0; i < n; ++i) {
        sum += arr[i];
        output[i] = sum;
    }
}
