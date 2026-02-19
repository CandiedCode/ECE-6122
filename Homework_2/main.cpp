#include <omp.h>
#include <stdio.h>

int main()
{
#pragma omp parallel
    {
        printf("Hello ECE 4122 6122\n");
    }

    return 0;
}
