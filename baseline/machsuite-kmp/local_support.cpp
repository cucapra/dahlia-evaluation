#include "xcl2.hpp"
#include <vector> 
#include "kmp.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_pattern = PATTERN_SIZE*sizeof(char);
  size_t vector_size_input = STRING_SIZE*sizeof(char);
  size_t vector_size_kmpNext = PATTERN_SIZE*sizeof(int32_t);
  size_t vector_size_n_matches = sizeof(int32_t);

  cl_int err;
  std::vector<char,aligned_allocator<char>> pattern(PATTERN_SIZE);
  std::vector<char,aligned_allocator<char>> input(STRING_SIZE);
  std::vector<int32_t,aligned_allocator<int32_t>> kmpNext(PATTERN_SIZE);
  std::vector<int32_t,aligned_allocator<int32_t>> n_matches(1);
  // Copy the test data 

  for(int i = 0 ; i < PATTERN_SIZE ; i++){
      pattern[i] = args->pattern[i];
      kmpNext[i] = args->kmpNext[i];
  }
  for(int i = 0 ; i < STRING_SIZE; i++) {
      input[i] = args->input[i];
  }
  
  // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err)); 

    // find_binary_file() is a utility API which will search the xclbin file for
    // targeted mode (sw_emu/hw_emu/hw) and for targeted platforms.
    std::string binaryFile = xcl::find_binary_file(device_name,"kmp");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_kmp(program,"kmp", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_pattern(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_pattern,
                                    pattern.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_input(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_input,
                                    input.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_kmpNext(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_kmpNext,
                                    kmpNext.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_n_matches(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_n_matches,
                                    n_matches.data(),
                                    &err));
    

    // Copy input data to device global memory d
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_pattern, buffer_input, buffer_kmpNext},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_kmp.setArg(0, buffer_pattern));
    OCL_CHECK(err, err = krnl_kmp.setArg(1, buffer_input));
    OCL_CHECK(err, err = krnl_kmp.setArg(2, buffer_kmpNext));
    OCL_CHECK(err, err = krnl_kmp.setArg(3, buffer_n_matches));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_kmp));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_n_matches},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  
  args->n_matches[0] =  n_matches[0];
  
}

/* Input format:
%% Section 1
char[PATTERN_SIZE]: pattern
%% Section 2
char[STRING_SIZE]: text
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_string(s, data->pattern, PATTERN_SIZE);

  s = find_section_start(p,2);
  parse_string(s, data->input, STRING_SIZE);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_string(fd, data->pattern, PATTERN_SIZE);

  write_section_header(fd);
  write_string(fd, data->input, STRING_SIZE);
}

/* Output format:
%% Section 1
int[1]: number of matches
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_int32_t_array(s, data->n_matches, 1);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd); // No section header
  write_int32_t_array(fd, data->n_matches, 1);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;

  has_errors |= (data->n_matches[0] != ref->n_matches[0]);

  // Return true if it's correct.
  return !has_errors;
}
