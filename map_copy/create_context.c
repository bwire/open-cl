#include <OpenCL/cl.h>

cl_context create_context(cl_device_id device) {
  cl_int error;
  
  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
  if(error < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  return context;
}