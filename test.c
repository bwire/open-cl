#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "matvec.cl"
#define KERNEL_FUNC "matvec_mult"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "OpenCL/cl.h"

int main() {
  cl_uint num_platforms;
  cl_platform_id* platforms;
  char information[128];
  size_t actual_size;

  clGetPlatformIDs(0, NULL, &num_platforms);

  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);
  clGetPlatformIDs(num_platforms, platforms, NULL);

  clGetPlatformInfo(platforms[0], CL_PLATFORM_VENDOR, sizeof(information), &information, &actual_size);
  printf("Platforms count: %i\n", num_platforms);
  printf("Platform: %s (size: %zu)\n", information, actual_size);

  return 0;
}

