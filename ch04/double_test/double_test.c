#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "double_test.cl"
#define KERNEL_FUNC "double_test"

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

cl_program build_program(cl_context context, cl_device_id device, 
  const char* filename, const char* options) {

  cl_program program;
  FILE *program_handle;
  size_t program_size, log_size;
  char *program_buffer, *program_log;
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

  program = clCreateProgramWithSource(context, 1, (const char**) &program_buffer, &program_size, &err);
  if(err < 0) {
    perror("Couldn't create the program");
    exit(1);
  }
  free(program_buffer);

  err = clBuildProgram(program, 0, NULL, options, NULL, NULL);
  if (err < 0) {
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*) malloc(log_size + 1);
    program_log[log_size] = '\0';

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, &log_size);
    printf("%s\n", program_log);
    free(program_log);
    exit(1);
  }

  return program;
} 

int main() {
  /* OpenCL data structures */
  cl_device_id device;
  cl_context context;
  cl_program program;
  cl_kernel kernel;
  cl_command_queue queue;

  /* Data and buffers */
  float a = 6.0, b = 2.0, result;
  cl_mem a_buffer, b_buffer, output_buffer;

  /* Extension data */
  cl_uint addr_data;
  char* ext_data;
  size_t ext_size;
  char fp64_ext[] = "cl_khr_fp64";
  char options[20] = "";

  cl_int err;

  /* Create a device and context */
  device = create_device();
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
  if(err < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  err = clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(addr_data), &addr_data, NULL);
  if (err) {
    perror("Couldn't read extension data");
    exit(1);
  }

  printf("Address width: %u\n", addr_data);

  /* Define "FP_64" option if doubles are supported */
  clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &ext_size);
  ext_data = (char*) malloc(ext_size + 1);
  ext_data[ext_size] = '\0';

  clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, ext_size + 1, ext_data, NULL);
  if (strstr(ext_data, fp64_ext) != NULL) {
    printf("The %s extension is supported.\n", fp64_ext);
    strcat(options, "-DFP_64 ");
  } else {
    printf("The %s extension is not supported.\n", fp64_ext);  
  }
  free(ext_data);

  /* Build the program and create the kernel */
  program = build_program(context, device, PROGRAM_FILE, options);
  kernel = clCreateKernel(program, KERNEL_FUNC, &err);
  if(err < 0) {
    perror("Couldn't create a kernel");
    exit(1);   
  };

  /* Create CL buffers to hold input and output data */
  a_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float), &a, &err);
  if(err < 0) {
    perror("Couldn't create a memory object for the first kernel arg");
    exit(1);   
  };
  b_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float), &b, &err);
  if(err < 0) {
    perror("Couldn't create a memory object for the second kernel arg");
    exit(1);   
  };
  output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, &err);
  if(err < 0) {
    perror("Couldn't allocate a memory object for the result");
    exit(1);   
  };

  /* Create kernel arguments */
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_buffer);
  if (!err) {
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_buffer); 
  }
  if (!err) {
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_buffer);
  }
  if(err < 0) {
    perror("Couldn't set kernel arguments");
    exit(1);   
  };

  /* Create a command queue */
  queue = clCreateCommandQueue(context, device, 0, &err);
  if(err < 0) {
    perror("Couldn't create a command queue");
    exit(1);   
  };

  /* Enqueue kernel */
  err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
  if(err < 0) {
    perror("Couldn't enqueue the kernel");
    exit(1);   
  }

  /* Read and print the result */
  err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, sizeof(float), &result, 0, NULL, NULL);
  if(err < 0) {
    perror("Couldn't read the output buffer");
    exit(1);   
  }

  printf("The kernel result is: %f\n", result);

  clReleaseCommandQueue(queue);
  clReleaseMemObject(a_buffer);
  clReleaseMemObject(b_buffer);
  clReleaseMemObject(output_buffer);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseContext(context);
}