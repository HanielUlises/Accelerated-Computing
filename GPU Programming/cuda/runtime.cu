#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

__global__ void test01() {
    printf("\nThe block IS is [%d] -- The thread ID is [%i]", blockIdx.x, threadIdx.x);
}

int main() {
    // kernel_name<<<num_of_blocks, num_of_threads_per_block>>>();
    test01 << <1,1>> > ();
    return 0;
}

