#include <omp.h>

#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

#include "msort.h"

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
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " n t ts\n";
        return 1;
    }

    std::size_t n;
    int t;
    std::size_t ts;
    try {
        n = static_cast<std::size_t>(parse_positive(argv[1], std::numeric_limits<long long>::max()));
        t = static_cast<int>(parse_positive(argv[2], std::numeric_limits<int>::max()));
        ts = static_cast<std::size_t>(parse_positive(argv[3], std::numeric_limits<long long>::max()));
    } catch (const std::exception&) {
        std::cerr << "n, t and ts must be positive integers.\n";
        return 1;
    }

    // n random integers in [-1000, 1000].
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);
    int* arr = new int[n];
    for (std::size_t i = 0; i < n; ++i) {
        arr[i] = dist(generator);
    }

    omp_set_num_threads(t);
    const auto start = high_resolution_clock::now();
    msort(arr, n, ts);
    const auto end = high_resolution_clock::now();
    const double elapsed_ms = duration_cast<duration<double, std::milli>>(end - start).count();

    std::cout << arr[0] << "\n";
    std::cout << arr[n - 1] << "\n";
    std::cout << elapsed_ms << "\n";

    delete[] arr;
    return 0;
}
