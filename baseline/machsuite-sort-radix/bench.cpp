#include "sort.h"
#include <string.h>
#include "xcl2.hpp"
#include <vector>

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_a = SIZE*sizeof(int);
  size_t vector_size_bytes_bucket = BUCKETSIZE*sizeof(int);
  size_t vector_size_bytes_sum = SCAN_RADIX*sizeof(int);
  
  cl_int err;
  std::vector<int,aligned_allocator<int>> a_in(SIZE);
  std::vector<int,aligned_allocator<int>> b_in(SIZE);
  std::vector<int,aligned_allocator<int>> bucket_in(SIZE);
  std::vector<int,aligned_allocator<int>> sum_in(SIZE);
  
  // Copy the test data 
  for(int i = 0 ; i < SIZE ; i++){
      a_in[i] = args->a[i];
      b_in[i] = args->b[i];
      bucket_in[i] = args->bucket[i];
      sum_in[i] = args->sum[i];
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
    std::string binaryFile = xcl::find_binary_file(device_name,"sort");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_merge_sort(program,"sort", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err, cl::Buffer buffer_a   (context,CL_MEM_USE_HOST_PTR |CL_MEM_READ_WRITE, 
            vector_size_bytes_a, a_in.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_b   (context,CL_MEM_USE_HOST_PTR |CL_MEM_READ_WRITE, 
            vector_size_bytes_a, b_in.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_bucket   (context,CL_MEM_USE_HOST_PTR |CL_MEM_READ_WRITE, 
            vector_size_bytes_bucket, bucket_in.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_sum   (context,CL_MEM_USE_HOST_PTR |CL_MEM_READ_WRITE, 
            vector_size_bytes_sum, sum_in.data(), &err));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b, buffer_bucket, buffer_sum},0/* 0 means from host*/));

    int size = SIZE;
    OCL_CHECK(err, err = krnl_merge_sort.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl_merge_sort.setArg(1, buffer_b));
    OCL_CHECK(err, err = krnl_merge_sort.setArg(2, buffer_bucket));
    OCL_CHECK(err, err = krnl_merge_sort.setArg(3, buffer_sum));



    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_merge_sort));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for(int i = 0 ; i < SIZE ; i++){
      args->a[i] = a_in[i];
  }
}

