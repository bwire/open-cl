#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/cl.h>

int main() {
  cl_platform_id platform;
  cl_device_info device;
  cl_device_fp_config config;
  cl_int err;

  err = clGetPlatformIDs(1, &platform, NULL);
  if (err < 0) {
  perror("Couldn't identify a platform");
  exit(1);
  }

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if (err == CL_DEVICE_NOT_FOUND) {
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);  
  }
  if(err < 0) {
    perror("Couldn't access any devices");
    exit(1);   
  }

  // check float-processing features
  err = clGetDeviceInfo(device, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(config), &config, NULL);
  if(err < 0) {
    perror("Couldn't read floating-point properties");
    exit(1);
  }

  printf("Float Processing Features:\n");
   if(config & CL_FP_INF_NAN) 
      printf("INF and NaN values supported.\n");
   if(config & CL_FP_DENORM) 
      printf("Denormalized numbers supported.\n");
   if(config & CL_FP_ROUND_TO_NEAREST) 
      printf("Round To Nearest Even mode supported.\n");
   if(config & CL_FP_ROUND_TO_INF) 
      printf("Round To Infinity mode supported.\n");
   if(config & CL_FP_ROUND_TO_ZERO) 
      printf("Round To Zero mode supported.\n");
   if(config & CL_FP_FMA) 
      printf("Floating-point multiply-and-add operation supported.\n"); 

  #ifndef MAC
   if(config & CL_FP_SOFT_FLOAT)
      printf("Basic floating-point processing performed in software.\n");
  #endif

  return 0; 
}