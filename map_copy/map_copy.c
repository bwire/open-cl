#define PROGRAM_FILE "blank.cl"

#include <OpenCL/cl.h>

#include "create_device.c"
#include "create_context.c"
#include "test_buffer.c"

int main() {
  cl_int err;
  float data_one[100], data_two[100];
  cl_mem buffer_one, buffer_two;

  cl_device_id device = create_device();
  cl_context context = create_context(device);

  test_buffer(device, context);
  
  clReleaseContext(context);
}