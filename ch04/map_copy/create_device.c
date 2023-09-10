#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/cl.h>

cl_device_id create_device() {
  cl_uint result;
  cl_platform_id platform;
  cl_device_id device;

  result = clGetPlatformIDs(1, &platform, NULL);
  if (result < 0) {
    perror("Couldn't identify a platform");
    exit(1);
  }

  result = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if (result == CL_DEVICE_NOT_FOUND) {
    result = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
  }
  if (result < 0) {
    perror("Couldn't access any devices");
    exit(1); 
  }

  return device;
}