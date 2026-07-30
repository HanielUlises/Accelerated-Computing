#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>

__global__ void test01() {
    // Print the blocks and threads IDs
    // warp=32 threads (128 threads/blocks) --> (128/32 = 4 warps/block)
    int warp_ID_value = 0;
    warp_ID_value = threadIdx.x / 32;
    printf("The block ID is %d --- The thread ID is %d --- The warp ID is %d\n", blockIdx.x, threadIdx.x, warp_ID_value);
}

int main() {
    // Addition fo two vectors ith 2048 elements
    // kernel_name<<<num_of_blocks, num_of_thread_per_block>>>();
    // test01 <<<1, 2048>>>(); --- warps/block=2 warps, total warps/GPU=4 warps
    test01<<<2, 64>>>();
    cudaDeviceSynchronize();
    return 0;
}