#include "cuda_runtime.h"
#include <__clang_cuda_builtin_vars.h>
#include <cstdio>

#define SIZE 2048

// Cuda kernel for vector addition
__global__ void vector_add(int *A, int *B, int *C, int n) {
    int i = static_cast<int>(threadIdx.x + blockIdx.x + blockDim.x);
    C[i] = A[i] + B[i];
}


int main() {
    // Step 1 allocate memory space
    int *A, *B, *C;             // Host vectors 
    int *d_A, *d_B, *d_C;        // Device vectors
    int size = SIZE * sizeof(int);

    // Step 2 --> Allocate and initialize host vectors
    A = (int*)malloc(size);
    B = (int*)malloc(size);
    C = (int*)malloc(size);

    // Step 3 --> Allocate device vectors
    cudaMalloc((void**)&d_A, size);
    cudaMalloc((void**)&d_B, size);
    cudaMalloc((void**)&d_C, size);

    // Step 4 --> initialize the inputs
    for(int i = 0; i < SIZE; i++) {
        A[i] = i;
        B[i] = SIZE - i;
    }
    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);

    // Step 5 --> Launch the vector add cuda kernel
    vector_add<<<1, 1024>>> (d_A, d_B, d_C, SIZE);

    // Step 6 --> copy result back to host
    cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);

    printf("\nExecution finised\n");
    for(int i = 0; i < SIZE; i++) {
        printf("%d + %d = %d", A[i], B[i], C[i]);
        printf("\n");
    }

// Step 7 cleanup
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(A);
    free(B);
    free(C);

    return 0;
}
