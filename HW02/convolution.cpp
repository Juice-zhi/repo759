#include "convolution.h"

#include <cstddef>

namespace {

// Returns f[i, j] for an n x n row-major image. Outside the image the value is
// 1 on the edges (exactly one index out of range) and 0 at the corners (both
// indices out of range).
float padded_pixel(const float* image, std::ptrdiff_t n, std::ptrdiff_t i, std::ptrdiff_t j) {
    const bool row_inside = 0 <= i && i < n;
    const bool col_inside = 0 <= j && j < n;
    if (row_inside && col_inside) {
        return image[i * n + j];
    }
    return (row_inside || col_inside) ? 1.0f : 0.0f;
}

}  // namespace

void convolve(const float* image, float* output, std::size_t n, const float* mask, std::size_t m) {
    // Signed indices, because x + i - half is negative near the top/left border.
    const std::ptrdiff_t size = static_cast<std::ptrdiff_t>(n);
    const std::ptrdiff_t width = static_cast<std::ptrdiff_t>(m);
    const std::ptrdiff_t half = (width - 1) / 2;

    for (std::ptrdiff_t x = 0; x < size; ++x) {
        for (std::ptrdiff_t y = 0; y < size; ++y) {
            float sum = 0.0f;
            for (std::ptrdiff_t i = 0; i < width; ++i) {
                for (std::ptrdiff_t j = 0; j < width; ++j) {
                    sum += mask[i * width + j] *
                           padded_pixel(image, size, x + i - half, y + j - half);
                }
            }
            output[x * size + y] = sum;
        }
    }
}
