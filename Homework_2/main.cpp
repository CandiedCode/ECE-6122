#include <omp.h>
#include <stdio.h>

auto main() -> int
{
#pragma omp parallel
    {
        printf("Hello ECE 4122 6122\n");
    }

    return 0;
}
