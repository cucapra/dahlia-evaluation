#include "xcl2.hpp"
#include <vector>
#include "stencil.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_orig = row_size*col_size*sizeof(TYPE);
  size_t vector_size_bytes_sol = row_size*col_size*sizeof(TYPE);
  size_t vector_size_bytes_filter = f_size*sizeof(TYPE);

  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> orig(row_size*col_size);
  std::vector<TYPE,aligned_allocator<TYPE>> sol(row_size*col_size);
  std::vector<TYPE,aligned_allocator<TYPE>> filter(f_size);
  
  // Copy the test data 
  for(int i = 0 ; i < row_size*col_size; i++){
      orig[i] = args->orig[i];
  }
   for(int i = 0 ; i < f_size; i++){
      filter[i] = args->filter[i];
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
    OCL_CHECK(err, cl::Kernel krnl_stencil_stencil2d(program,"stencil", &err));

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
              cl::Buffer buffer_filter(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes_filter,
                                    filter.data(),
                                    &err));
   
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_orig, buffer_filter},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_stencil_stencil2d.setArg(0, buffer_orig));
    OCL_CHECK(err, err = krnl_stencil_stencil2d.setArg(1, buffer_sol));
    OCL_CHECK(err, err = krnl_stencil_stencil2d.setArg(2, buffer_filter));


    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_stencil_stencil2d));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_sol},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for(int i = 0 ; i < row_size*col_size  ; i++){
      args->sol[i] = sol[i];
  }
}
