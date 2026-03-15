#include <stdio.h>
#include <omp.h>

int main() {

    printf("Before parallel region\n");

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello from thread %d\n", tid);
    }

    printf("All threads joined. Back to single thread.\n");

    return 0;
}