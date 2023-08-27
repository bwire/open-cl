#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "OpenCL/cl.h"

int main() {
  cl_uint num_platforms;
  cl_platform_id* platforms;
  cl_int platform_index = -1;
  const char icd_ext[] = "cl_khr_icd";
  

  cl_uint result = clGetPlatformIDs(0, NULL, &num_platforms);
  if (result < 0) {
    perror("Couldn't find any platform");
    exit(1);
  }

  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);
  clGetPlatformIDs(num_platforms, platforms, NULL);

  for (cl_int i = 0; i < num_platforms; i++) {
    size_t extension_size;
    
    cl_int result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 0, NULL, &extension_size);
    if (result < 0) {
      perror("Couldn't read extension data");
      exit(1);
    }

    char* extension_data = (char*) malloc(extension_size);
    clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, extension_size, extension_data, NULL);
    printf("Platform %d supports extensions: %s\n", i, extension_data);

    if (strstr(extension_data, icd_ext) != NULL) {
      platform_index = i;
      free(extension_data);
      break;  
    }
    free(extension_data);
  }

  if (platform_index > 0) {
    printf("Platform %d supports the %s extension", platform_index, icd_ext);
  } else {
    printf("No platform supports the %s extension", icd_ext);
  }

  free(platforms);
  return 0;
}

