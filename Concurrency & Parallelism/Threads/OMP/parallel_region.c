#include <stdio.h>
#include <omp.h>

int main() {

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < 10; i++) {
            printf("Thread %d handles iteration %d\n", tid, i);
        }
    }

    return 0;
}