#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 1000000
#define BINS 10

int main() {
    int *data = malloc(DATA_SIZE * sizeof(int));
    int histogram[BINS] = {0};

    srand(time(NULL));

    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % BINS;
    }

    #pragma omp parallel
    {
        int local_hist[BINS] = {0};

        #pragma omp for
        for (int i = 0; i < DATA_SIZE; i++) {
            local_hist[data[i]]++;
        }

        // Merge results safely
        #pragma omp critical
        {
            for (int i = 0; i < BINS; i++) {
                histogram[i] += local_hist[i];
            }
        }
    }

    for (int i = 0; i < BINS; i++) {
        printf("Bin %d: %d\n", i, histogram[i]);
    }

    free(data);
    return 0;
}