#include <CL/cl.hpp>
#include <vector>
#include <iostream>
#include <cmath>

const char* kernelSource = R"CLC(
__kernel void fvm_diffusion_step(__global const float* u,
                                 __global float* u_new,
                                 const float alpha,
                                 const float dx,
                                 const float dy,
                                 const float dt,
                                 const int nx,
                                 const int ny)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i <= 0 || j <= 0 || i >= nx - 1 || j >= ny - 1)
        return;

    int idx = j * nx + i;

    float uC = u[idx];
    float uE = u[j * nx + (i + 1)];
    float uW = u[j * nx + (i - 1)];
    float uN = u[(j + 1) * nx + i];
    float uS = u[(j - 1) * nx + i];

    float dudx2 = (uE - 2.0f * uC + uW) / (dx * dx);
    float dudy2 = (uN - 2.0f * uC + uS) / (dy * dy);

    u_new[idx] = uC + dt * alpha * (dudx2 + dudy2);
}
)CLC";

int main()
{
    const int nx = 256;
    const int ny = 256;
    const float dx = 1.0f / nx;
    const float dy = 1.0f / ny;
    const float alpha = 0.01f;
    const float dt = 0.25f * std::min(dx * dx, dy * dy) / alpha;
    const int steps = 500;

    std::vector<float> u(nx * ny, 0.0f);
    std::vector<float> u_new(nx * ny, 0.0f);

    for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i)
            if ((i - nx/2)*(i - nx/2) + (j - ny/2)*(j - ny/2) < 400)
                u[j * nx + i] = 1.0f;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();

    cl::Context context(device);
    cl::Program program(context, kernelSource);
    program.build({device});

    cl::CommandQueue queue(context, device);

    cl::Buffer bufferU(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * u.size(), u.data());
    cl::Buffer bufferUNew(context, CL_MEM_READ_WRITE, sizeof(float) * u_new.size());

    cl::Kernel kernel(program, "fvm_diffusion_step");
    kernel.setArg(2, alpha);
    kernel.setArg(3, dx);
    kernel.setArg(4, dy);
    kernel.setArg(5, dt);
    kernel.setArg(6, nx);
    kernel.setArg(7, ny);

    cl::NDRange global(nx, ny);

    for (int s = 0; s < steps; ++s)
    {
        kernel.setArg(0, bufferU);
        kernel.setArg(1, bufferUNew);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, global);
        std::swap(bufferU, bufferUNew);
    }

    queue.enqueueReadBuffer(bufferU, CL_TRUE, 0, sizeof(float) * u.size(), u.data());

    std::cout << u[(ny/2) * nx + nx/2] << std::endl;

    return 0;
}