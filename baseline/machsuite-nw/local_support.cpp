#include "xcl2.hpp"
#include <vector>
#include "nw.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_SEQA = ALEN*sizeof(char);
  size_t vector_size_bytes_SEQB = BLEN*sizeof(char);
  size_t vector_size_bytes_alignedA = (ALEN+BLEN)*sizeof(char);
  size_t vector_size_bytes_alignedB = (ALEN+BLEN)*sizeof(char);
  size_t vector_size_bytes_M = ((ALEN+1)*(BLEN+1))*sizeof(int);
  size_t vector_size_bytes_ptr = ((ALEN+1)*(BLEN+1))*sizeof(char);

  cl_int err;
  std::vector<char,aligned_allocator<char>> SEQA(ALEN);
  std::vector<char,aligned_allocator<char>> SEQB(BLEN);
  std::vector<char,aligned_allocator<char>> alignedA(ALEN+BLEN);
  std::vector<char,aligned_allocator<char>> alignedB(ALEN+BLEN);
  std::vector<int,aligned_allocator<int>> M((ALEN+1)*(BLEN+1));
  std::vector<char,aligned_allocator<char>> ptr((ALEN+1)*(BLEN+1));
  
  // Copy the test data 
  for(int i = 0 ; i < ALEN ; i++){
      SEQA[i] = args->seqA[i];
  }
  for(int i = 0 ; i < BLEN ; i++){
      SEQB[i] = args->seqB[i];
  }
  for(int i = 0 ; i < ALEN + BLEN; i++){
      alignedA[i] = args->alignedA[i];
      alignedB[i] = args->alignedB[i];
  }
  for(int i = 0 ; i < (ALEN+1)*(BLEN+1) ; i++){
      M[i] = args->M[i];
      ptr[i] = args->ptr[i];
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
    std::string binaryFile = xcl::find_binary_file(device_name,"nw");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_nw(program,"nw", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_SEQA(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes_SEQA,
                                    SEQA.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_SEQB(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes_SEQB,
                                    SEQB.data(),
                                    &err));

    OCL_CHECK(err,
              cl::Buffer buffer_alignedA(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                    vector_size_bytes_alignedA,
                                    alignedA.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_alignedB(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes_alignedB,
                                       alignedB.data(),
                                       &err));
    OCL_CHECK(err,
              cl::Buffer buffer_M(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                       vector_size_bytes_M,
                                       M.data(),
                                       &err));
    OCL_CHECK(err,
              cl::Buffer buffer_ptr(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                       vector_size_bytes_ptr,
                                       ptr.data(),
                                       &err));
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_SEQA, buffer_SEQB, buffer_M, buffer_ptr},0/* 0 means from host*/));

    OCL_CHECK(err, err = krnl_nw.setArg(0, buffer_SEQA));
    OCL_CHECK(err, err = krnl_nw.setArg(1, buffer_SEQB));
    OCL_CHECK(err, err = krnl_nw.setArg(2, buffer_alignedA));
    OCL_CHECK(err, err = krnl_nw.setArg(3, buffer_alignedB));
    OCL_CHECK(err, err = krnl_nw.setArg(4, buffer_M));
    OCL_CHECK(err, err = krnl_nw.setArg(5, buffer_ptr));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_nw));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_alignedA, buffer_alignedB},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for(int i = 0 ; i < ALEN+BLEN  ; i++){
      args->alignedA[i] = alignedA[i];
      args->alignedB[i] = alignedB[i];
  }
}


/* Input format:s
%% Section 1
char[]: sequence A
%% Section 2
char[]: sequence B
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_string(s, data->seqA, ALEN);

  s = find_section_start(p,2);
  parse_string(s, data->seqB, BLEN);
  free(p);

}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_string(fd, data->seqA, ALEN);

  write_section_header(fd);
  write_string(fd, data->seqB, BLEN);

  write_section_header(fd);
}

/* Output format:
%% Section 1
char[sum_size]: aligned sequence A
%% Section 2
char[sum_size]: aligned sequence B
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_string(s, data->alignedA, ALEN+BLEN);

  s = find_section_start(p,2);
  parse_string(s, data->alignedB, ALEN+BLEN);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_string(fd, data->alignedA, ALEN+BLEN);

  write_section_header(fd);
  write_string(fd, data->alignedB, ALEN+BLEN);

  write_section_header(fd);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;

  has_errors |= memcmp(data->alignedA, ref->alignedA, ALEN+BLEN);
  has_errors |= memcmp(data->alignedB, ref->alignedB, ALEN+BLEN);

  // Return true if it's correct.
  return !has_errors;
}
