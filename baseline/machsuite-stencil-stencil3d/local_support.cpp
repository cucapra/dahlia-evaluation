#include "stencil.h"
#include <string>
#include "xcl2.hpp"
#include <vector>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON (1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_orig = SIZE*sizeof(TYPE);
  size_t vector_size_bytes_sol = SIZE*sizeof(TYPE);
  size_t vector_size_bytes_C = 2*sizeof(TYPE);

  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> orig(SIZE);
  std::vector<TYPE,aligned_allocator<TYPE>> sol(SIZE);
  std::vector<TYPE,aligned_allocator<TYPE>> C(2);

  // Copy the test data
  for(int i = 0 ; i < SIZE; i++){
      orig[i] = args->orig[i];
  }
   for(int i = 0 ; i < 2; i++){
      C[i] = args->C[i];
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
    std::string binaryFile = xcl::find_binary_file(device_name,"stencil");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_stencil_stencil3d(program,"stencil", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_orig(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_orig,
                                    orig.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_sol(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_sol,
                                    sol.data(),
                                    &err));

    OCL_CHECK(err,
              cl::Buffer buffer_C(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_C,
                                    C.data(),
                                    &err));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_orig, buffer_C},0/* 0 means from host*/));

    OCL_CHECK(err, err = krnl_stencil_stencil3d.setArg(0, buffer_C));
    OCL_CHECK(err, err = krnl_stencil_stencil3d.setArg(1, buffer_orig));
    OCL_CHECK(err, err = krnl_stencil_stencil3d.setArg(2, buffer_sol));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    cl::Event event;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err, err = q.enqueueTask(krnl_stencil_stencil3d, NULL, &event));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_sol},CL_MIGRATE_MEM_OBJECT_HOST));
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
  for(int i = 0 ; i < SIZE  ; i++){
      args->sol[i] = sol[i];
  }
}

/* Input format:
%% Section 1
TYPE[2]: stencil coefficients (inner/outer)
%% Section 2
TYPE[SIZE]: input matrix
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->C, 2);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->orig, SIZE);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->C, 2);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->orig, SIZE);
}

/* Output format:
%% Section 1
TYPE[SIZE]: solution matrix
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->sol, SIZE);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->sol, SIZE);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  TYPE diff;

  for(i=0; i<SIZE; i++) {
    diff = data->sol[i] - ref->sol[i];
    has_errors |= (diff<-EPSILON) || (EPSILON<diff);
  }

  // Return true if it's correct.
  return !has_errors;
}
