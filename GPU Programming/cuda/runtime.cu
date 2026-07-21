#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

__global__ void test01() {

}

int main() {
    test01 << <1,1>> > ();
    return 0;
}

