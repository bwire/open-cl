#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OpenCL/cl.h>

int main() {
  cl_uint operation_result;
  cl_platform_id platform_id;
  cl_device_id device_id;
  cl_int error;
  cl_uint ref_count;
  
  operation_result = clGetPlatformIDs(1, &platform_id, NULL); 
  if (operation_result < 0) {
    perror("Couldn't obtain platform information");
    exit(1);
  }

  operation_result = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
  if (operation_result == CL_DEVICE_NOT_FOUND) {
    operation_result = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);  
  }
  if (operation_result < 0) {
    perror("Couldn't find any device");
    exit(1); 
  }

  cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &error);
  if(error < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  operation_result = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);
  if(operation_result < 0) {		
    perror("Couldn't read the reference count.");
    exit(1);
  }

  printf("Initial reference count: %u\n", ref_count);

  clRetainContext(context);
  clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);
  printf("Reference count (after retain): %u\n", ref_count);

  clReleaseContext(context);						
   clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);			
   printf("Reference count (after release): %u\n", ref_count);	

  clReleaseContext(context);
  return 0;
}