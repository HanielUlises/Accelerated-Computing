#include <cuda_runtime.h>
#include <cstdio>

template<int BLOCK_SIZE, bool USE_SHARED>
__global__ void kernel(float* data, int n)
{
    const int tid = threadIdx.x;
    const int idx = blockIdx.x * BLOCK_SIZE + tid;

    if constexpr (USE_SHARED) {
        __shared__ float scratch[BLOCK_SIZE];

        if (idx < n)
            scratch[tid] = data[idx];

        __syncthreads();

        if (idx < n)
            data[idx] = scratch[tid];
    }
    else {
        if (idx < n)
            data[idx] *= 2.0f;
    }
}