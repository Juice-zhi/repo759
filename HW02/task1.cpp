#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "scan.h"

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

namespace {

// Parses a command line argument that must be a positive integer.
std::size_t parse_positive(const char* text) {
    const std::string argument = text;
    std::size_t parsed = 0;
    const long long value = std::stoll(argument, &parsed);
    if (parsed != argument.size() || value <= 0) {
        throw std::invalid_argument(argument);
    }
    return static_cast<std::size_t>(value);
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " n\n";
        return 1;
    }

    std::size_t n;
    try {
        n = parse_positive(argv[1]);
    } catch (const std::exception&) {
        std::cerr << "n must be a positive integer.\n";
        return 1;
    }

    // n random floats in [-1, 1].
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    float* arr = new float[n];
    float* output = new float[n];
    for (std::size_t i = 0; i < n; ++i) {
        arr[i] = dist(generator);
    }

    const auto start = high_resolution_clock::now();
    scan(arr, output, n);
    const auto end = high_resolution_clock::now();
    const double elapsed_ms = duration_cast<duration<double, std::milli>>(end - start).count();

    std::cout << elapsed_ms << "\n";
    std::cout << output[0] << "\n";
    std::cout << output[n - 1] << "\n";

    delete[] arr;
    delete[] output;
    return 0;
}
