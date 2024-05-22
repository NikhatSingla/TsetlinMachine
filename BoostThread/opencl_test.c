#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cl_uint platformCount;
    cl_platform_id* platforms;

    // Get platform count
    clGetPlatformIDs(0, NULL, &platformCount);

    // Get all platforms
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);

    // Print platform names
    for (cl_uint i = 0; i < platformCount; i++) {
        char buffer[10240];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(buffer), buffer, NULL);
        printf("Platform %d: %s\n", i, buffer);
    }

    free(platforms);
    return 0;
}
