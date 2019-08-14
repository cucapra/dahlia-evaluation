#include "xcl2.hpp"
#include <vector>
#include "spmv.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_nzval = N*L*sizeof(TYPE);
  size_t vector_size_bytes_cols = N*L*sizeof(int32_t);
  size_t vector_size_bytes_vec = N*sizeof(TYPE);
  size_t vector_size_bytes_out = N*sizeof(TYPE);

  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> nzval_in(N*L);
  std::vector<int32_t,aligned_allocator<int32_t>> cols_in(N*L);
  std::vector<TYPE,aligned_allocator<TYPE>> vec_in(N);
  std::vector<TYPE,aligned_allocator<TYPE>> out(N);
  
  // Copy the test data 
  for(int i = 0 ; i < N*L ; i++){
      nzval_in[i] = args->nzval[i];
      cols_in[i] = args->cols[i];
      if (i < N) {
      vec_in[i] = args->vec[i];
      out[i] = args->out[i];
      }
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
    std::string binaryFile = xcl::find_binary_file(device_name,"spmv");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_spmv_ellpack(program,"spmv", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_val(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_nzval,
                                    nzval_in.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_cols(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_cols,
                                    cols_in.data(),
                                    &err));

    OCL_CHECK(err,
              cl::Buffer buffer_vec(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_vec,
                                    vec_in.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes_out,
                                       out.data(),
                                       &err));


    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_val, buffer_cols, buffer_vec, buffer_output},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_spmv_ellpack.setArg(0, buffer_val));
    OCL_CHECK(err, err = krnl_spmv_ellpack.setArg(1, buffer_cols));
    OCL_CHECK(err, err = krnl_spmv_ellpack.setArg(2, buffer_vec));
    OCL_CHECK(err, err = krnl_spmv_ellpack.setArg(3, buffer_output));


    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_spmv_ellpack));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for(int i = 0 ; i < N  ; i++){
      args->out[i] = out[i];
  }
}
