#include <omp.h>
#include <stdio.h>

int main() {
    int sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 1000000; i++) {
        sum += i;  // Safe
    }

    printf("Sum = %d\n", sum);
    return 0;
}