#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000

int main() {
    double **matrix;
    double total_sum = 0.0;

    // Allocate matrix
    matrix = (double **)malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++) {
        matrix[i] = (double *)malloc(N * sizeof(double));
    }

    // Initialize matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = (double)(i + j);
        }
    }

    // Parallel sum with reduction
    #pragma omp parallel for collapse(2) reduction(+:total_sum)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            total_sum += matrix[i][j];
        }
    }

    printf("Total sum = %.2f\n", total_sum);

    // Free memory
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}