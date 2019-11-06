
#include <string>
#include <vector>
#include "xcl2.hpp"
#include "helpers.hpp"
#include "gemm.h"

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes = N*sizeof(TYPE);
  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> m_in1(N);
  std::vector<TYPE,aligned_allocator<TYPE>> m_in2(N);
  std::vector<TYPE,aligned_allocator<TYPE>> m_prod(N);

  // Copy the test data
  for(int i = 0; i < N; i++){
    m_in1[i] = args->m1[i];
    m_in2[i] = args->m2[i];
  }

  // OPENCL HOST CODE AREA START
  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

  cl::Kernel krnl_gemm_ncube = helpers::get_kernel(context, device, "gemm", err);

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
      cl::Buffer buffer_in1(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        vector_size_bytes,
        m_in1.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer buffer_in2(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        vector_size_bytes,
        m_in2.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer buffer_output(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        vector_size_bytes,
        m_prod.data(),
        &err));


  OCL_CHECK(err, err = krnl_gemm_ncube.setArg(0, buffer_in1));
  OCL_CHECK(err, err = krnl_gemm_ncube.setArg(1, buffer_in2));
  OCL_CHECK(err, err = krnl_gemm_ncube.setArg(2, buffer_output));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},0));

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_gemm_ncube, NULL, &event));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
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
  for(int i = 0; i < N; i++){
    args->prod[i] = m_prod[i];
  }

}

/* Input format:
%% Section 1
TYPE[N]: matrix 1
%% Section 2
TYPE[N]: matrix 2
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->m1, N);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->m2, N);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->m1, N);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->m2, N);
}

/* Output format:
%% Section 1
TYPE[N]: output matrix
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->prod, N);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->prod, N);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int r,c;
  TYPE diff;

  for( r=0; r<row_size; r++ ) {
    for( c=0; c<col_size; c++ ) {
      diff = data->prod[r*row_size + c] - ref->prod[r*row_size+c];
      has_errors |= (diff<-EPSILON) || (EPSILON<diff);
    }
  }

  // Return true if it's correct.
  return !has_errors;
}
