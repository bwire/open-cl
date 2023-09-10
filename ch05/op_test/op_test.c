#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "op_test.cl"
#define KERNEL_FUNC "op_test"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OpenCL/cl.h>

cl_device_id create_device() {
  cl_platform_id platform;
  cl_device_id device;
  cl_int err;

  err = clGetPlatformIDs(1, &platform, NULL); 
  if(err < 0) {
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
  return device;
}

cl_program build_program(cl_context ctx, cl_device_id device, const char* filename) {
  cl_program program;
  FILE *program_handle;
  char *program_buffer, *program_log;
  size_t program_size, log_size;
  cl_int err;

  program_handle = fopen(filename, "r");
  if(program_handle == NULL) {
    perror("Couldn't find the program file");
    exit(1);
  }

  fseek(program_handle, 0, SEEK_END);
  program_size = ftell(program_handle);
  rewind(program_handle);

  program_buffer = (char*) malloc(program_size + 1);
  program_buffer[program_size] = '\0';

  fread(program_buffer, sizeof(char), program_size, program_handle);
  fclose(program_handle);

  program = clCreateProgramWithSource(ctx, 1, (const char**) &program_buffer, &program_size, &err);
  if(err < 0) {
    perror("Couldn't create the program");
    exit(1);
  }
  free(program_buffer);

  // build
  err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  if (err < 0) {
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*) malloc(log_size + 1);
    program_log[log_size] = '\0';

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL); 
    printf("%s\n", program_log);
    free(program_log);
    exit(1);
  }

  return program;
}


int main() {
  cl_device_id device;
  cl_context context;
  cl_kernel kernel;
  cl_command_queue queue;
  cl_program program;

  int test[4];
  cl_mem test_buffer;

  cl_int i, err;

  device = create_device();
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  if(err < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }
  program = build_program(context, device, PROGRAM_FILE);
  kernel = clCreateKernel(program, KERNEL_FUNC, &err);
  if(err < 0) {
    perror("Couldn't create a kernel");
    exit(1);   
  };

  test_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(test), NULL, &err);
  if(err < 0) {
    perror("Couldn't create a buffer");
    exit(1);   
  };

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &test_buffer);
  if(err < 0) {
    perror("Couldn't set a kernel argument");
    exit(1);   
  };

  queue = clCreateCommandQueue(context, device, 0, &err);
  if(err < 0) {
    perror("Couldn't create a command queue");
    exit(1);   
  };

  err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
  if(err < 0) {
    perror("Couldn't enqueue the kernel");
    exit(1);   
  }

  err = clEnqueueReadBuffer(queue, test_buffer, CL_TRUE, 0, sizeof(test), &test, 0, NULL, NULL);
  if(err < 0) {
    perror("Couldn't read the buffer");
    exit(1);   
  }

  for (i = 0; i < 4; i++) {
    printf("%d, ", test[i]);
  }
  
  clReleaseCommandQueue(queue);
  clReleaseMemObject(test_buffer);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseContext(context);
  return 0;
}