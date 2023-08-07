#define PROGRAM_FILE "blank.cl"

#include <OpenCL/cl.h>

#include "create_device.c"
#include "create_context.c"
#include "test_buffer.c"
#include "build_program.c"

int main() {
  cl_int err;

  cl_device_id device = create_device();
  cl_context context = create_context(device);
  cl_program program = build_program(context, device, PROGRAM_FILE);

  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
  if (err < 0) {
    perror("Could not create a command queue");
    exit(1);
  }

  cl_kernel kernel = clCreateKernel(program, "blank", &err);
  test_buffer(context, queue, kernel);

  
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(context);
}