#include <iostream>
#include <omp.h>

auto main() -> int
{
#pragma omp parallel
    {
        std::cout << "Hello ECE 4122 6122\n";
    }

    return 0;
}
