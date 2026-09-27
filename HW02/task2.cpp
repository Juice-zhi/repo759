#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "convolution.h"

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
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " n m\n";
        return 1;
    }

    std::size_t n;
    std::size_t m;
    try {
        n = parse_positive(argv[1]);
        m = parse_positive(argv[2]);
    } catch (const std::exception&) {
        std::cerr << "n and m must be positive integers.\n";
        return 1;
    }
    if (m % 2 == 0) {
        std::cerr << "m must be odd.\n";
        return 1;
    }

    std::mt19937 generator(std::random_device{}());

    // n x n image with entries in [-10, 10], row-major.
    std::uniform_real_distribution<float> image_dist(-10.0f, 10.0f);
    float* image = new float[n * n];
    for (std::size_t i = 0; i < n * n; ++i) {
        image[i] = image_dist(generator);
    }

    // m x m mask with entries in [-1, 1], row-major.
    std::uniform_real_distribution<float> mask_dist(-1.0f, 1.0f);
    float* mask = new float[m * m];
    for (std::size_t i = 0; i < m * m; ++i) {
        mask[i] = mask_dist(generator);
    }

    float* output = new float[n * n];
    const auto start = high_resolution_clock::now();
    convolve(image, output, n, mask, m);
    const auto end = high_resolution_clock::now();
    const double elapsed_ms = duration_cast<duration<double, std::milli>>(end - start).count();

    std::cout << elapsed_ms << "\n";
    std::cout << output[0] << "\n";
    std::cout << output[n * n - 1] << "\n";

    delete[] image;
    delete[] mask;
    delete[] output;
    return 0;
}
