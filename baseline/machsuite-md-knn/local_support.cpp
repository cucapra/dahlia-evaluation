#include "md.h"
#include "xcl2.hpp"
#include "helpers.hpp"
#include <vector>
#include <string>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
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
  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_md_knn, NULL, &event));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err,
      err = q.enqueueMigrateMemObjects(
        {force_x_buffer, force_y_buffer, force_z_buffer},CL_MIGRATE_MEM_OBJECT_HOST));

  q.finish();

  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
  OCL_CHECK(err,
            err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));

  auto t = (nstimeend - nstimestart)/1000000.0;
  std::cout << "Iteration: " << iter << ": " << t << " ms." << std::endl;
  *runtime << iter << "," << t << std::endl;

  // Copy results
  for (int i=0; i < NATOMS; i++) {
    args->force_x[i] = force_x[i];
    args->force_y[i] = force_y[i];
    args->force_z[i]  = force_z[i] ;
  }
}

/* Input format:
%% Section 1
TYPE[NATOMS]: x positions
%% Section 2
TYPE[NATOMS]: y positions
%% Section 3
TYPE[NATOMS]: z positions
%% Section 4
int32_t[NATOMS*MAXNEIGHBORS]: neighbor list
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->position_x, NATOMS);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->position_y, NATOMS);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->position_z, NATOMS);

  s = find_section_start(p,4);
  parse_int32_t_array(s, data->NL, NATOMS*MAXNEIGHBORS);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_x, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_y, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->position_z, NATOMS);

  write_section_header(fd);
  write_int32_t_array(fd, data->NL, NATOMS*MAXNEIGHBORS);

}

/* Output format:
%% Section 1
TYPE[NATOMS]: new x force
%% Section 2
TYPE[NATOMS]: new y force
%% Section 3
TYPE[NATOMS]: new z force
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->force_x, NATOMS);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->force_y, NATOMS);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->force_z, NATOMS);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_x, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_y, NATOMS);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->force_z, NATOMS);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  TYPE diff_x, diff_y, diff_z;

  for( i=0; i<NATOMS; i++ ) {
    diff_x = data->force_x[i] - ref->force_x[i];
    diff_y = data->force_y[i] - ref->force_y[i];
    diff_z = data->force_z[i] - ref->force_z[i];
    has_errors |= (diff_x<-EPSILON) || (EPSILON<diff_x);
    has_errors |= (diff_y<-EPSILON) || (EPSILON<diff_y);
    has_errors |= (diff_z<-EPSILON) || (EPSILON<diff_z);
  }

  // Return true if it's correct.
  return !has_errors;
}
