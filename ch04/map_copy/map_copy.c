#define PROGRAM_FILE_BUFFERS "buffers.cl"

#include <stdio.h>
#include <string.h>
#include <OpenCL/cl.h>

#include "create_device.c"
#include "create_context.c"
#include "test_buffer.c"

int main() {
  cl_int i, err;
  float data_one[100], data_two[100], result_array[1000];
  cl_mem buffer_one, buffer_two;
  char param[1000];
  void *mapped_memory;

  cl_device_id device = create_device();
  cl_context context = create_context(device);

  //test_buffer(device, context);

  // prepare data
  for(i = 0; i < 100; i++) {
    data_one[i] = i * 1.0f;
  } 

  cl_program program = build_program(context, device, PROGRAM_FILE_BUFFERS);
  cl_kernel kernel = clCreateKernel(program, "buff", &err);

  //err = clGetKernelInfo(kernel,  CL_KERNEL_FUNCTION_NAME, sizeof(param), &param[0], NULL);

  buffer_one = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
  if (err < 0) {
    perror("Couldn'r create a buffer 1 object");
    exit(1);
  }

  buffer_two = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);
  if (err < 0) {
    perror("Couldn'r create a buffer 2 object");
    exit(1);
  }

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_one);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_two);
  if (err < 0) {
    perror("Couldn't set kernel arguments");
    exit(1);
  }

  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
  if (err < 0) {
    perror("Could not create a command queue");
    exit(1);
  }
 
  err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
  if (err < 0) {
    perror("Could not enqueue the kernel");
    exit(1);
  }

  err = clEnqueueCopyBuffer(queue, buffer_one, buffer_two, 0, 0, sizeof(data_one), 0, NULL, NULL);
  if (err < 0) {
    perror("Could not perform copying");
    exit(1);
  }

  mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE, CL_MAP_READ, 0, sizeof(data_two), 0, NULL, NULL, &err);
  if (err < 0) {
    perror("Could not map memory");
    exit(1);
  }

  memcpy(result_array, mapped_memory, sizeof(data_two));

  err = clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory, 0, NULL, NULL);
  if (err < 0) {
    perror("Could not unmap memory");
    exit(1);
  }

  clReleaseCommandQueue(queue);
  clReleaseMemObject(buffer_one);
  clReleaseMemObject(buffer_two);
  clReleaseKernel(kernel);
  clReleaseContext(context);
}