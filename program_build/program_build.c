#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"

#include <stdlib.h>
#include <stdio.h>
#include <OpenCL/cl.h>

int main() {
  cl_int i, result, error;
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_program program;

  FILE* file_handle;
  const char* file_names[] = {PROGRAM_FILE_1, PROGRAM_FILE_2};
  const char options[] = "-cl-finite-math-only -cl-no-signed-zeros"; 
  size_t program_size[NUM_FILES];
  char* program_buffer[NUM_FILES];
  size_t log_size;
  char* program_log;

  // we only need 1 platform for the demonstration purposes
  result = clGetPlatformIDs(1, &platform, NULL);
  if (result < 0) {
    perror("Could not find any platform");
    exit(1);
  }

  result = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
  if (result == CL_DEVICE_NOT_FOUND) {
    result = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
  }
  if (result < 0) {
    perror("Could not find any device");
    exit(1);
  }

  context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
  if(error < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  for(i = 0; i < NUM_FILES; i++) {
    file_handle = fopen(file_names[i], "r");
    if (file_handle == NULL) {
      perror("Error opening file");
      exit(1);
    }
    fseek(file_handle, 0, SEEK_END);
    program_size[i] = ftell(file_handle);
    rewind(file_handle);

    program_buffer[i] = (char*) malloc(program_size[i] + 1);
    program_buffer[i][program_size[i]] = '\0';
    fread(program_buffer[i], sizeof(char), program_size[i], file_handle);
    fclose(file_handle);
  }

  program = clCreateProgramWithSource(context, NUM_FILES, (const char**) program_buffer, program_size, &error);
  if(error < 0) {
    perror("Couldn't create the program");
    exit(1);   
  }

  result = clBuildProgram(program, 1, &device, options, NULL, NULL);
  if (result < 0) {
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    program_log = (char*) malloc(log_size + 1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
    printf("%s\n", program_log);
    free(program_log);
    exit(1);
  }

  for(i = 0; i < NUM_FILES; i++) {
    free(program_buffer[i]);
  }
  clReleaseProgram(program);
  clReleaseContext(context);
}