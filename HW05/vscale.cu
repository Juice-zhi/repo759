#include "vscale.cuh"

// Thread i, counted across the whole grid, computes b[i] = a[i] * b[i];
// threads past the end of the arrays do nothing.
__global__ void vscale(const float* a, float* b, unsigned int n) {
    const unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        b[i] = a[i] * b[i];
    }
}
