#define PROGRAM_FILE "blank.cl"

#include <stdio.h>
#include <OpenCL/cl.h>

#include "create_device.c"
#include "build_program.c"

int main() {
  cl_int error;
  cl_device_id device;
  cl_program program;
  
  device = create_device();

  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
  if(error < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  program = build_program(context, device, PROGRAM_FILE);
}