#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/cl.h>

cl_program build_program(cl_context ctx, cl_device_id device, const char* filename) {
  cl_int error;
  cl_program program;
  FILE *program_handle;
  char *program_buffer, *program_log;
  size_t program_size, log_size;

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
  fread(program_buffer, sizeof(char), program_size + 1, program_handle);
  fclose(program_handle);

  program = clCreateProgramWithSource(ctx, 1, (const char**) &program_buffer, &program_size, &error);
  if(error < 0) {
    perror("Couldn't create the program");
    exit(1);
  }
  free(program_buffer);

  error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (error < 0) {
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
