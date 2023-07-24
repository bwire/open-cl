#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "OpenCL/cl.h"

int main() {
  cl_uint operation_result;
  cl_uint num_platforms;
  cl_platform_id* platforms;
  cl_int platform_index = -1;
  
  operation_result = clGetPlatformIDs(0, NULL, &num_platforms);
  if (operation_result < 0) {
    perror("Couldn't find any platform");
    exit(1);
  }

  platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);
  clGetPlatformIDs(num_platforms, platforms, NULL);

  for (cl_int i = 0; i < num_platforms; i++) {
    cl_device_id* devices;
    cl_uint num_devices;    

    operation_result = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices); 
    if (operation_result < 0) {
      perror("Couldn't find any device");
      exit(1);
    }

    devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);
    clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

    for (cl_int j = 0; j < num_devices; j++) {
      size_t param_value_size;
      cl_uint address_width;

      operation_result = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 1, NULL, &param_value_size);  
      if (operation_result < 0) {
        perror("Couldn't get device name size");
        exit(1);
      }

      char* device_name = (char*) malloc(param_value_size);
      operation_result = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, param_value_size, device_name, NULL);  
      if (operation_result < 0) {
        perror("Couldn't get device name");
        exit(1);
      }

      operation_result = clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &address_width, NULL);  
      if (operation_result < 0) {
        perror("Couldn't get address space size");
        exit(1);
      }

      operation_result = clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, 0, NULL, &param_value_size);  
      if (operation_result < 0) {
        perror("Couldn't get extensions names size");
        exit(1);
      }

      char* extensions = (char*) malloc(param_value_size);
      operation_result = clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, param_value_size, extensions, NULL);  
      if (operation_result < 0) {
        perror("Couldn't get extensions names");
        exit(1);
      }

      printf("NAME: %s\nADDRESS WIDTH: %u\nEXTENSIONS: %s", device_name, address_width, extensions);

      free(extensions);
      free(device_name);
    }

    free(devices);
  }

  free(platforms);
  return 0;
}

