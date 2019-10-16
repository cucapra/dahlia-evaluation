
#include <string.h>
#include "xcl2.hpp"
#include "helpers.hpp"
#include <vector>
#include "gemm.h"

void run_benchmark( void *vargs ) {
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
  OCL_CHECK(err, err = q.enqueueTask(krnl_gemm_ncube));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  // OPENCL HOST CODE AREA END

  // Copy results
  for(int i = 0; i < N; i++){
    args->prod[i] = m_prod[i];
  }

}
