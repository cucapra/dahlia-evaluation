#include "xcl2.hpp"
#include "helpers.hpp"
#include <vector>
#include "md.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;

  cl_int err;

  std::vector<TYPE,aligned_allocator<TYPE>> force_x(NATOMS);
  std::vector<TYPE,aligned_allocator<TYPE>> force_y(NATOMS);
  std::vector<TYPE,aligned_allocator<TYPE>> force_z(NATOMS);
  std::vector<TYPE,aligned_allocator<TYPE>> position_x(NATOMS);
  std::vector<TYPE,aligned_allocator<TYPE>> position_y(NATOMS);
  std::vector<TYPE,aligned_allocator<TYPE>> position_z(NATOMS);
  std::vector<int32_t,aligned_allocator<int32_t>> NL(NATOMS*MAXNEIGHBORS);

  for (int i=0; i < NATOMS; i++) {
    force_x[i]  = args->force_x[i] ;
    force_y[i]  = args->force_y[i] ;
    force_z[i] = args->force_z[i] ;
    position_x[i]  = args->position_x[i] ;
    position_y[i] = args->position_y[i] ;
    position_z[i]  = args->position_z[i] ;
  }

  for(int i = 0; i < NATOMS*MAXNEIGHBORS; i++) {
    NL[i] = args->NL[i];
  }

  // OPENCL HOST CODE AREA START
  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

  cl::Kernel krnl_md_knn = helpers::get_kernel(context, device, "md", err);

  // Argument buffers
  // Read only buffers
  OCL_CHECK(err,
      cl::Buffer nl_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        NATOMS*MAXNEIGHBORS*sizeof(int32_t),
        NL.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer position_x_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        NATOMS*sizeof(TYPE),
        position_x.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer position_y_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        NATOMS*sizeof(TYPE),
        position_y.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer position_z_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        NATOMS*sizeof(TYPE),
        position_z.data(),
        &err));

  // Write only buffers
  OCL_CHECK(err,
      cl::Buffer force_x_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        NATOMS*sizeof(TYPE),
        force_x.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer force_y_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        NATOMS*sizeof(TYPE),
        force_y.data(),
        &err));
  OCL_CHECK(err,
      cl::Buffer force_z_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        NATOMS*sizeof(TYPE),
        force_z.data(),
        &err));

  OCL_CHECK(err, err = krnl_md_knn.setArg(0, force_x_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(1, force_y_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(2, force_z_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(3, position_x_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(4, position_y_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(5, position_z_buffer));
  OCL_CHECK(err, err = krnl_md_knn.setArg(6, nl_buffer));

  // Copy input data to device global memory
  OCL_CHECK(err,
      err = q.enqueueMigrateMemObjects(
        {position_x_buffer, position_y_buffer, position_z_buffer, nl_buffer}, 0));

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  OCL_CHECK(err,
      err = q.enqueueTask(krnl_md_knn));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err,
      err = q.enqueueMigrateMemObjects(
        {force_x_buffer, force_y_buffer, force_z_buffer},CL_MIGRATE_MEM_OBJECT_HOST));

  q.finish();

  // Copy results
  for (int i=0; i < NATOMS; i++) {
    args->force_x[i] = force_x[i];
    args->force_y[i] = force_y[i];
    args->force_z[i]  = force_z[i] ;
  }
}
