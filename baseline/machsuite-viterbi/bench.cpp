#include "viterbi.h"
#include "xcl2.hpp"
#include <vector>

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  cl_int err;

  std::vector<tok_t,aligned_allocator<tok_t>> obs(N_OBS);
  std::vector<prob_t,aligned_allocator<prob_t>> init(N_STATES);
  std::vector<prob_t,aligned_allocator<prob_t>> transition(N_STATES*N_STATES);
  std::vector<prob_t,aligned_allocator<prob_t>> emission(N_STATES*N_TOKENS);
  std::vector<state_t,aligned_allocator<state_t>> path(N_OBS);

  for (int i=0; i < N_OBS; i++) obs[i] = args->obs[i];
  for (int i=0; i < N_STATES; i++) init[i] = args->init[i];
  for (int i=0; i < N_STATES*N_STATES; i++) transition[i] = args->transition[i];
  for (int i=0; i < N_STATES*N_TOKENS; i++) emission[i] = args->emission[i];     
        

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
    std::string binaryFile = xcl::find_binary_file(device_name,"viterbi");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_viterbi(program,"viterbi", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer obs_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_OBS*sizeof(tok_t),
                                    obs.data(),
                                    &err));  
    OCL_CHECK(err,
              cl::Buffer init_buffer(context, 
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_STATES*sizeof(prob_t),
                                    init.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer transition_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_STATES*N_STATES*sizeof(prob_t),
                                    transition.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer emission_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_STATES*N_TOKENS*sizeof(prob_t),
                                    emission.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer path_buffer(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    N_OBS*sizeof(state_t),
                                    path.data(),
                                    &err));  
    
   
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({obs_buffer, transition_buffer, emission_buffer, init_buffer},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_viterbi.setArg(0, obs_buffer));
    OCL_CHECK(err, err = krnl_viterbi.setArg(1, init_buffer));
    OCL_CHECK(err, err = krnl_viterbi.setArg(2, transition_buffer));
    OCL_CHECK(err, err = krnl_viterbi.setArg(3, emission_buffer));
    OCL_CHECK(err, err = krnl_viterbi.setArg(4, path_buffer));


    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_viterbi));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({path_buffer},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for (int i=0; i < N_OBS; i++) args->path[i] = path[i];
}

