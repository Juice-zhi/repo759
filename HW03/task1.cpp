#include <omp.h>

#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

#include "matmul.h"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

namespace {

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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " n t\n";
        return 1;
    }

    std::size_t n;
    int t;
    try {
        n = static_cast<std::size_t>(parse_positive(argv[1], std::numeric_limits<long long>::max()));
        t = static_cast<int>(parse_positive(argv[2], std::numeric_limits<int>::max()));
    } catch (const std::exception&) {
        std::cerr << "n and t must be positive integers.\n";
        return 1;
    }

    // Random n x n matrices with entries in [-1, 1], row-major.
    const std::size_t size = n * n;
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    float* A = new float[size];
    float* B = new float[size];
    float* C = new float[size];
    for (std::size_t i = 0; i < size; ++i) {
        A[i] = dist(generator);
        B[i] = dist(generator);
    }

    omp_set_num_threads(t);
    const auto start = high_resolution_clock::now();
    mmul(A, B, C, n);
    const auto end = high_resolution_clock::now();
    const double elapsed_ms = duration_cast<duration<double, std::milli>>(end - start).count();

    std::cout << C[0] << "\n";
    std::cout << C[size - 1] << "\n";
    std::cout << elapsed_ms << "\n";

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
