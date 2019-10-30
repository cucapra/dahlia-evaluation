#include "viterbi.h"
#include <string>
#include "xcl2.hpp"
#include "helpers.hpp"
#include <vector>

int INPUT_SIZE = sizeof(struct bench_args_t);

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
  for (int i=0; i < N_STATES*N_STATES; i++) {
     transition[i] = args->transition[i];
     emission[i] = args->emission[i];
  }

  // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    cl::Kernel krnl_viterbi = helpers::get_kernel(context, device, "viterbi", err);

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
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    N_OBS*sizeof(state_t),
                                    path.data(),
                                    &err));


    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({obs_buffer, transition_buffer, emission_buffer, init_buffer},0/* 0 means from host*/));

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

/* Input format:
%% Section 1
tok_t[N_OBS]: observation vector
%% Section 2
prob_t[N_STATES]: initial state probabilities
%% Section 3
prob_t[N_STATES*N_STATES]: transition matrix
%% Section 4
prob_t[N_STATES*N_TOKENS]: emission matrix
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_uint8_t_array(s, data->obs, N_OBS);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->init, N_STATES);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->transition, N_STATES*N_STATES);

  s = find_section_start(p,4);
  STAC(parse_,TYPE,_array)(s, data->emission, N_STATES*N_TOKENS);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_uint8_t_array(fd, data->obs, N_OBS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->init, N_STATES);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->transition, N_STATES*N_STATES);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->emission, N_STATES*N_TOKENS);
}

/* Output format:
%% Section 1
uint8_t[N_OBS]: most likely state chain
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_uint8_t_array(s, data->path, N_OBS);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_uint8_t_array(fd, data->path, N_OBS);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;

  for(i=0; i<N_OBS; i++) {
    has_errors |= (data->path[i]!=ref->path[i]);
  }

  // Return true if it's correct.
  return !has_errors;
}
