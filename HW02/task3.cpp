#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

#include "matmul.h"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

int main() {
    const unsigned int n = 1024;
    const std::size_t size = static_cast<std::size_t>(n) * n;

    // Random n x n matrices in row-major order. mmul1-mmul3 take raw arrays and
    // mmul4 takes std::vector, so the same values are stored in both forms.
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    double* A = new double[size];
    double* B = new double[size];
    for (std::size_t i = 0; i < size; ++i) {
        A[i] = dist(generator);
        B[i] = dist(generator);
    }
    const std::vector<double> A_vec(A, A + size);
    const std::vector<double> B_vec(B, B + size);
    double* C = new double[size];

    // Runs one multiplication, then prints its time in ms and the last entry of C.
    const auto time_and_print = [&](auto multiply) {
        const auto start = high_resolution_clock::now();
        multiply();
        const auto end = high_resolution_clock::now();
        std::cout << duration_cast<duration<double, std::milli>>(end - start).count() << "\n";
        std::cout << C[size - 1] << "\n";
    };

    std::cout << n << "\n";
    time_and_print([&] { mmul1(A, B, C, n); });
    time_and_print([&] { mmul2(A, B, C, n); });
    time_and_print([&] { mmul3(A, B, C, n); });
    time_and_print([&] { mmul4(A_vec, B_vec, C, n); });

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
