#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

#include "vscale.cuh"

namespace {

// Exits with a message if a CUDA call failed.
void check(cudaError_t status) {
    if (status != cudaSuccess) {
        std::fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(status));
        std::exit(EXIT_FAILURE);
    }
}

// Parses a command line argument that must be an integer in [1, max_value].
long long parse_positive(const char* text, long long max_value) {
    const std::string argument = text;
    std::size_t parsed = 0;
    const long long value = std::stoll(argument, &parsed);
    if (parsed != argument.size() || value < 1 || value > max_value) {
        throw std::invalid_argument(argument);
    }
    return value;
}

}  // namespace

// Usage: ./task3 n [threads_per_block]
// threads_per_block defaults to 512; the scaling study also runs it with 16.
int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        std::fprintf(stderr, "Usage: %s n [threads_per_block]\n", argv[0]);
        return 1;
    }

    unsigned int n;
    unsigned int threads_per_block = 512;
    try {
        n = static_cast<unsigned int>(
            parse_positive(argv[1], std::numeric_limits<unsigned int>::max()));
        if (argc == 3) {
            threads_per_block = static_cast<unsigned int>(parse_positive(argv[2], 1024));
        }
    } catch (const std::exception&) {
        std::fprintf(stderr,
                     "n must be a positive integer and threads_per_block an integer in [1, 1024].\n");
        return 1;
    }

    // a with entries in [-10, 10], b with entries in [0, 1].
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist_a(-10.0f, 10.0f);
    std::uniform_real_distribution<float> dist_b(0.0f, 1.0f);
    float* hA = new float[n];
    float* hB = new float[n];
    for (unsigned int i = 0; i < n; ++i) {
        hA[i] = dist_a(generator);
        hB[i] = dist_b(generator);
    }

    const std::size_t bytes = static_cast<std::size_t>(n) * sizeof(float);
    float* dA;
    float* dB;
    check(cudaMalloc(&dA, bytes));
    check(cudaMalloc(&dB, bytes));
    check(cudaMemcpy(dA, hA, bytes, cudaMemcpyHostToDevice));
    check(cudaMemcpy(dB, hB, bytes, cudaMemcpyHostToDevice));

    // Launch once with n = 0, so that no thread does any work, to keep the
    // one-time cost of loading the kernel onto the GPU out of the timed launch.
    vscale<<<1, threads_per_block>>>(dA, dB, 0);
    check(cudaGetLastError());
    check(cudaDeviceSynchronize());

    // One thread per element: enough blocks to cover all n elements.
    const unsigned int num_blocks = n / threads_per_block + (n % threads_per_block != 0);
    cudaEvent_t start;
    cudaEvent_t stop;
    check(cudaEventCreate(&start));
    check(cudaEventCreate(&stop));

    check(cudaEventRecord(start));
    vscale<<<num_blocks, threads_per_block>>>(dA, dB, n);
    check(cudaEventRecord(stop));
    check(cudaEventSynchronize(stop));
    check(cudaGetLastError());

    float elapsed_ms = 0.0f;
    check(cudaEventElapsedTime(&elapsed_ms, start, stop));
    check(cudaMemcpy(hB, dB, bytes, cudaMemcpyDeviceToHost));

    std::printf("%g\n", elapsed_ms);
    std::printf("%g\n", hB[0]);
    std::printf("%g\n", hB[n - 1]);

    check(cudaEventDestroy(start));
    check(cudaEventDestroy(stop));
    check(cudaFree(dA));
    check(cudaFree(dB));
    delete[] hA;
    delete[] hB;
    return 0;
}
