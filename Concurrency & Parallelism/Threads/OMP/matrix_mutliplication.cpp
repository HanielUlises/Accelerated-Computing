#include <stdio.h>
#include <omp.h>

#define N 4

int main() {

    int A[N][N], B[N][N], C[N][N];

    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            A[i][j] = i + j;
            B[i][j] = i * j;
            C[i][j] = 0;
        }
    }

    #pragma omp parallel for collapse(2)
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            for(int k=0;k<N;k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    printf("Result matrix:\n");
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}