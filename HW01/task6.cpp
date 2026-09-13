#include <cstdio>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    int n;
    try {
        const std::string argument = argv[1];
        std::size_t parsed = 0;
        const long long value = std::stoll(argument, &parsed);
        if (parsed != argument.size() || value < 0 ||
            value > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("N");
        }
        n = static_cast<int>(value);
    } catch (const std::exception&) {
        std::cerr << "N must be a non-negative integer in the int range.\n";
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        std::printf("%d ", i);
    }
    std::printf("%d\n", n);

    for (int i = n; i > 0; --i) {
        std::cout << i << ' ';
    }
    std::cout << 0 << '\n';

    return 0;
}
