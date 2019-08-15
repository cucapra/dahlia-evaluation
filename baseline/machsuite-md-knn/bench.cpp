#include "xcl2.hpp"
#include <vector>
#include "md.h"
void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  // size_t vector_size_bytes_orig = row_size*col_size*sizeof(TYPE);
  // size_t vector_size_bytes_sol = row_size*col_size*sizeof(TYPE);
  // size_t vector_size_bytes_filter = f_size*sizeof(TYPE);

  cl_int err;
  // std::vector<TYPE,aligned_allocator<TYPE>> orig(row_size*col_size);
  // std::vector<TYPE,aligned_allocator<TYPE>> sol(row_size*col_size);
  // std::vector<TYPE,aligned_allocator<TYPE>> filter(f_size);

  // Copy the test data 
  // for(int i = 0 ; i < row_size*col_size; i++){
  //     orig[i] = args->orig[i];
  // }
  //  for(int i = 0 ; i < f_size; i++){
  //     filter[i] = args->filter[i];
  // }
  std::vector<TYPE,aligned_allocator<TYPE>> force_x(nAtoms);
  std::vector<TYPE,aligned_allocator<TYPE>> force_y(nAtoms);
  std::vector<TYPE,aligned_allocator<TYPE>> force_z(nAtoms);
  std::vector<TYPE,aligned_allocator<TYPE>> position_x(nAtoms);
  std::vector<TYPE,aligned_allocator<TYPE>> position_y(nAtoms);
  std::vector<TYPE,aligned_allocator<TYPE>> position_z(nAtoms);
  std::vector<int32_t,aligned_allocator<int32_t>> NL(nAtoms*maxNeighbors);



  for (int i=0; i<nAtoms; i++) 
        {
          force_x[i]  = (args->force_x)[i] ;
          force_y[i]  = (args->force_y)[i] ;
          force_z[i] = (args->force_z)[i] ;
          position_x[i]  = (args->position_x)[i] ;
          position_y[i] = (args->position_y)[i] ;
          position_z[i]  = (args->position_z)[i] ;
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
    std::string binaryFile = xcl::find_binary_file(device_name,"md");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_md_knn(program,"md", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer nl_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(NL),
                                    NL.data(),
                                    &err));  
    OCL_CHECK(err,
              cl::Buffer force_x_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(force_x),
                                    force_x.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer force_y_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(force_y),
                                    force_y.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer force_z_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(force_z),
                                    force_z.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer position_x_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(position_x),
                                    position_x.data(),
                                    &err));  
    
    OCL_CHECK(err,
              cl::Buffer position_y_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(position_y),
                                    position_y.data(),
                                    &err));  
    OCL_CHECK(err,
              cl::Buffer position_z_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    sizeof(position_z),
                                    position_z.data(),
                                    &err));  
    
   
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({position_x_buffer, position_y_buffer, position_z_buffer, nl_buffer},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_md_knn.setArg(0, force_x_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(1, force_y_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(2, force_z_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(3, position_x_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(4, position_y_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(5, position_z_buffer));
    OCL_CHECK(err, err = krnl_md_knn.setArg(6, nl_buffer));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_md_knn));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({force_x_buffer, force_y_buffer, force_z_buffer},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for (int i=0; i < nAtoms; i++) 
              {
                args->force_x[i] = force_x[i];
                args->force_y[i] = force_y[i];
                args->force_z[i]  = force_z[i] ;
              }
}
