#include <stdlib.h>
#include <stdio.h>
#include <OpenCL/cl.h>

void test_buffer(cl_context ctx, cl_command_queue queue,  cl_kernel kernel) {
  cl_int i, err;
  float full_matrix[80], zero_matrix[80];
  cl_mem matrix_buffer;
  const size_t buffer_origin[3] = {5 * sizeof(float), 3, 0};
  const size_t host_origin[3] = {sizeof(float), 1, 0};
  const size_t region[3] = {4 * sizeof(float), 4, 1};
  const size_t row_pitch = 10;

  for(i = 0; i < 80; i++) {
    full_matrix[i] = i * 2.0f;
    zero_matrix[i] = 0.0f;
   } 

  matrix_buffer = clCreateBuffer(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(full_matrix), full_matrix, &err);
  if (err < 0) {
    perror("Couldn't create a buffer");
    exit(1);
  }

  err = clSetKernelArg(kernel, 0, sizeof(matrix_buffer), &matrix_buffer);
  if (err < 0) {
    perror("Couldn't set the buffer as the kernel argument");
    exit(1);  
  }

  err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
  if (err < 0) {
    perror("Couldn't put the kernel into the queue");
    exit(1);  
  }

  err = clEnqueueWriteBuffer(queue, matrix_buffer, CL_TRUE, 0, sizeof(full_matrix), full_matrix, 0, NULL, NULL);
  if (err < 0) {
    perror("Couldn't write to the buffer");
    exit(1);  
  }

  err = clEnqueueReadBufferRect(
    queue, 
    matrix_buffer, 
    CL_TRUE,  
    buffer_origin, 
    host_origin, 
    region, 
    10 * sizeof(float),
    0,
    10 * sizeof(float),
    0,
    zero_matrix,
    0, 
    NULL,
    NULL
  );

  clReleaseMemObject(matrix_buffer);
}