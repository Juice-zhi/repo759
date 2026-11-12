#include <cstdio>
#include <cstdlib>
#include <random>

namespace {

// Exits with a message if a CUDA call failed.
void check(cudaError_t status) {
    if (status != cudaSuccess) {
        std::fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(status));
        std::exit(EXIT_FAILURE);
    }
}

}  // namespace

// Every thread writes a * x + y to its own entry of dA, where x is its
// threadIdx and y is its blockIdx.
__global__ void compute(int* dA, int a) {
    const int x = static_cast<int>(threadIdx.x);
    const int y = static_cast<int>(blockIdx.x);
    dA[blockIdx.x * blockDim.x + threadIdx.x] = a * x + y;
}

int main() {
    const int num_blocks = 2;
    const int threads_per_block = 8;
    const int n = num_blocks * threads_per_block;

    // Random integer a in [1, 100].
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 100);
    const int a = dist(generator);

    int* dA;
    check(cudaMalloc(&dA, n * sizeof(int)));
    compute<<<num_blocks, threads_per_block>>>(dA, a);
    check(cudaGetLastError());

    // cudaMemcpy waits for the kernel to finish before copying.
    int hA[n];
    check(cudaMemcpy(hA, dA, n * sizeof(int), cudaMemcpyDeviceToHost));
    check(cudaFree(dA));

    std::printf("%d", hA[0]);
    for (int i = 1; i < n; ++i) {
        std::printf(" %d", hA[i]);
    }
    std::printf("\n");
    return 0;
}
