#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <iostream>

int main() {
    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);

    if (platformCount == 0) {
        std::cout << "No OpenCL platforms found." << std::endl;
        return 1;
    }

    std::cout << "Number of OpenCL platforms: " << platformCount << std::endl;

    cl_platform_id* platforms = new cl_platform_id[platformCount];
    clGetPlatformIDs(platformCount, platforms, nullptr);

    for (cl_uint i = 0; i < platformCount; ++i) {
        char buffer[1024];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buffer), buffer, nullptr);
        std::cout << "Platform " << i + 1 << ": " << buffer << std::endl;

        cl_uint deviceCount;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceCount);
        if (deviceCount == 0) {
            std::cout << "  No devices found." << std::endl;
        } else {
            std::cout << "  Number of devices: " << deviceCount << std::endl;
        }
    }

    delete[] platforms;
    return 0;
}
