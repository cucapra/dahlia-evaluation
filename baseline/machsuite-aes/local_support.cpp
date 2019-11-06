#include <vector>
#include "xcl2.hpp"
#include "aes.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_key = 32*sizeof(uint8_t);
  size_t vector_size_bytes_enckey = 32*sizeof(uint8_t);
  size_t vector_size_bytes_deckey = 32*sizeof(uint8_t);
  size_t vector_size_bytes_k = 32*sizeof(uint8_t);
  size_t vector_size_bytes_buf = 16*sizeof(uint8_t);

  cl_int err;
  std::vector<uint8_t,aligned_allocator<uint8_t>> key(32);
  std::vector<uint8_t,aligned_allocator<uint8_t>> enckey(32);
  std::vector<uint8_t,aligned_allocator<uint8_t>> deckey(32);
  std::vector<uint8_t,aligned_allocator<uint8_t>> k(32);
  std::vector<uint8_t,aligned_allocator<uint8_t>> buf(16);

  // Copy the test data
  for(int i = 0 ; i < 32; i++){
    if(i< 16) {
      buf[i] = args->buf[i];
    }
    key[i] = args->ctx.key[i];
    enckey[i] = args->ctx.enckey[i];
    deckey[i] = args->ctx.deckey[i];
    k[i] = args->k[i];
  }
  // OPENCL HOST CODE AREA START
  // platforms and will return list of devices connected to Xilinx platform
  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
  OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

  // find_binary_file() is a utility API which will search the xclbin file for
  // targeted mode (sw_emu/hw_emu/hw) and for targeted platforms.
  std::string binaryFile = xcl::find_binary_file(device_name,"aes");

  // import_binary_file() ia a utility API which will load the binaryFile
  // and will return Binaries.
  cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
  devices.resize(1);
  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  OCL_CHECK(err, cl::Kernel krnl_aes(program,"aes", &err));

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
            cl::Buffer buffer_key(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                  vector_size_bytes_key,
                                  key.data(),
                                  &err));
  OCL_CHECK(err,
            cl::Buffer buffer_enckey(context,
                                     CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                     vector_size_bytes_enckey,
                                     enckey.data(),
                                     &err));
  OCL_CHECK(err,
            cl::Buffer buffer_deckey(context,
                                     CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                     vector_size_bytes_deckey,
                                     deckey.data(),
                                     &err));

  OCL_CHECK(err,
            cl::Buffer buffer_k(context,
                                CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                vector_size_bytes_k,
                                k.data(),
                                &err));

  OCL_CHECK(err,
            cl::Buffer buffer_buf(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                  vector_size_bytes_buf,
                                  buf.data(),
                                  &err));

  // Copy input data to device global memory d
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_key, buffer_enckey, buffer_deckey, buffer_k, buffer_buf},0/* 0 means from host*/));

  OCL_CHECK(err, err = krnl_aes.setArg(0, buffer_key));
  OCL_CHECK(err, err = krnl_aes.setArg(1, buffer_enckey));
  OCL_CHECK(err, err = krnl_aes.setArg(2, buffer_deckey));
  OCL_CHECK(err, err = krnl_aes.setArg(3, buffer_k));
  OCL_CHECK(err, err = krnl_aes.setArg(4, buffer_buf));
  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_aes, NULL, &event));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_buf},CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  // OPENCL HOST CODE AREA END

  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));

  auto t = (nstimeend - nstimestart)/1000000.0;
  std::cout << "Iteration: " << iter << ": " << t << " ms." << std::endl;
  *runtime << iter << "," << t << std::endl;

  // Copy results
  for(int i = 0 ; i < 16 ; i++){
    args->buf[i] =  buf[i];
  }

}

/* Input format:
%%: Section 1
uint8_t[32]: key
%%: Section 2
uint8_t[16]: input-text
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);
  // Section 1: key
  s = find_section_start(p,1);
  parse_uint8_t_array(s, data->k, 32);
  // Section 2: input-text
  s = find_section_start(p,2);
  parse_uint8_t_array(s, data->buf, 16);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  // Section 1
  write_section_header(fd);
  write_uint8_t_array(fd, data->k, 32);
  // Section 2
  write_section_header(fd);
  write_uint8_t_array(fd, data->buf, 16);
}

/* Output format:
%% Section 1
uint8_t[16]: output-text
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);
  // Section 1: output-text
  s = find_section_start(p,1);
  parse_uint8_t_array(s, data->buf, 16);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  // Section 1
  write_section_header(fd);
  write_uint8_t_array(fd, data->buf, 16);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;

  // Exact compare encrypted output buffers
  has_errors |= memcmp(&data->buf, &ref->buf, 16*sizeof(uint8_t));

  // Return true if it's correct.
  return !has_errors;
}
