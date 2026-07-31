#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdio>

__global__ void test01() {
    // print the blocks and threads IDs
    int warp_ID_value = 0;
    warp_ID_value = static_cast<int>(threadIdx.x / 32);
    printf("The block ID is %d --- The thread ID is %d --- The warp ID is %d\n", blockIdx.x , threadIdx.x, warp_ID_value);
}

int main() {
    test01<<<1, 128>>>();
    cudaDeviceSynchronize();

    return 0;
}
