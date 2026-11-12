#include <cstdio>
#include <cstdlib>

// Thread i computes a! for a = i + 1 and prints "a!=b".
__global__ void factorial() {
    const int a = static_cast<int>(threadIdx.x) + 1;
    int b = 1;
    for (int k = 2; k <= a; ++k) {
        b *= k;
    }
    std::printf("%d!=%d\n", a, b);
}

int main() {
    factorial<<<1, 8>>>();

    // Wait for the kernel (and its printf output) to finish before returning.
    const cudaError_t status = cudaDeviceSynchronize();
    if (status != cudaSuccess) {
        std::fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(status));
        return EXIT_FAILURE;
    }
    return 0;
}
