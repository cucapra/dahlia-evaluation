#include "xcl2.hpp"
#include <vector>
#include "md.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
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
  TYPE force_x[blockSide][blockSide][blockSide][densityFactor];
  TYPE force_y[blockSide][blockSide][blockSide][densityFactor];
  TYPE force_z[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_x[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_y[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_z[blockSide][blockSide][blockSide][densityFactor];

  int i,j,k,l;
  for (i=0; i<blockSide; i++)
    for (j=0; j<blockSide; j++)
      for (k=0; k<blockSide; k++)
        for (l=0; l<densityFactor; l++)
          {
            force_x[i][j][k][l] = (args->force_x)[i][j][k][l];
            force_y[i][j][k][l] = (args->force_y)[i][j][k][l];
            force_z[i][j][k][l] = (args->force_z)[i][j][k][l];
            position_x[i][j][k][l] = (args->position_x)[i][j][k][l];
            position_y[i][j][k][l] = (args->position_y)[i][j][k][l];
            position_z[i][j][k][l] = (args->position_z)[i][j][k][l];
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
  OCL_CHECK(err, cl::Kernel krnl_md_grid(program,"md", &err));

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
            cl::Buffer n_points_buffer(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                       sizeof(args->n_points),
                                       args->n_points,
                                       &err));
  OCL_CHECK(err,
            cl::Buffer force_x_buffer(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      sizeof(force_x),
                                      force_x,
                                      &err));
  OCL_CHECK(err,
            cl::Buffer force_y_buffer(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      sizeof(force_y),
                                      force_y,
                                      &err));
  OCL_CHECK(err,
            cl::Buffer force_z_buffer(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      sizeof(force_z),
                                      force_z,
                                      &err));
  OCL_CHECK(err,
            cl::Buffer position_x_buffer(context,
                                         CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                         sizeof(position_x),
                                         position_x,
                                         &err));

  OCL_CHECK(err,
            cl::Buffer position_y_buffer(context,
                                         CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                         sizeof(position_y),
                                         position_y,
                                         &err));
  OCL_CHECK(err,
            cl::Buffer position_z_buffer(context,
                                         CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                         sizeof(position_z),
                                         position_z,
                                         &err));


  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({position_x_buffer, position_y_buffer, position_z_buffer, n_points_buffer},0/* 0 means from host*/));

  OCL_CHECK(err, err = krnl_md_grid.setArg(0, n_points_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(1, force_x_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(2, force_y_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(3, force_z_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(4, position_x_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(5, position_y_buffer));
  OCL_CHECK(err, err = krnl_md_grid.setArg(6, position_z_buffer));

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_md_grid, NULL, &event));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({force_x_buffer, force_y_buffer, force_z_buffer},CL_MIGRATE_MEM_OBJECT_HOST));
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
  for (i=0; i<blockSide; i++)
    for (j=0; j<blockSide; j++)
      for (k=0; k<blockSide; k++)
        for (l=0; l<densityFactor; l++)
          {
            (args->force_x)[i][j][k][l] = force_x[i][j][k][l];
            (args->force_y)[i][j][k][l] = force_y[i][j][k][l];
            (args->force_z)[i][j][k][l] = force_z[i][j][k][l];
          }
}

/* Input format:
%% Section 1
int32_t[blockSide^3]: grid populations
%% Section 2
TYPE[blockSide^3*densityFactor]: positions
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  dvector_t *position;

  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_int32_t_array(s, (int32_t *)(data->n_points), blockSide*blockSide*blockSide);

  s = find_section_start(p,2);
  position = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  STAC(parse_,TYPE,_array)(s, (double *)(position), 3*blockSide*blockSide*blockSide*densityFactor);
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        data->position_x[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x;
        data->position_y[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y;
        data->position_z[i][j][k][l] = position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z;
    }
  }}}
  free(position);
  //STAC(parse_,TYPE,_array)(s, (double *)(data->position), 3*blockSide*blockSide*blockSide*densityFactor);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  dvector_t *position;

  write_section_header(fd);
  write_int32_t_array(fd, (int32_t *)(data->n_points), blockSide*blockSide*blockSide);

  write_section_header(fd);
  position = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x = data->position_x[i][j][k][l];
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y = data->position_y[i][j][k][l];
        position[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z = data->position_z[i][j][k][l];
    }
  }}}
  STAC(write_,TYPE,_array)(fd, (double *)(position), 3*blockSide*blockSide*blockSide*densityFactor);
//  STAC(write_,TYPE,_array)(fd, (double *)(data->position), 3*blockSide*blockSide*blockSide*densityFactor);
  free(position);

}

/* Output format:
%% Section 1
TYPE[blockSide^3*densityFactor]: force
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  dvector_t *force;

  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  force = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  STAC(parse_,TYPE,_array)(s, (double *)force, 3*blockSide*blockSide*blockSide*densityFactor);
//  STAC(parse_,TYPE,_array)(s, (double *)data->force, 3*blockSide*blockSide*blockSide*densityFactor);
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        data->force_x[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x;
        data->force_y[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y;
        data->force_z[i][j][k][l] = force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z;
    }
  }}}
  free(force);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  dvector_t *force;

  write_section_header(fd);
  force = (dvector_t *)malloc(nBlocks*densityFactor*sizeof(dvector_t));
  for(int i = 0; i < blockSide; i++) {
  for(int j = 0; j < blockSide; j++) {
  for(int k = 0; k < blockSide; k++) {
    for(int l = 0; l < densityFactor; l++) {
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].x = data->force_x[i][j][k][l];
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].y = data->force_y[i][j][k][l];
        force[(i*blockSide*blockSide+j*blockSide+k)*densityFactor+l].z = data->force_z[i][j][k][l];
    }
  }}}
  STAC(write_,TYPE,_array)(fd, (double *)force, 3*blockSide*blockSide*blockSide*densityFactor);
  free(force);
  //STAC(write_,TYPE,_array)(fd, (double *)data->force, 3*blockSide*blockSide*blockSide*densityFactor);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i, j, k, d;
  TYPE diff_x, diff_y, diff_z;

  for(i=0; i<blockSide; i++) {
    for(j=0; j<blockSide; j++) {
      for(k=0; k<blockSide; k++) {
        for(d=0; d<densityFactor; d++) {
          diff_x = data->force_x[i][j][k][d] - ref->force_x[i][j][k][d];
          diff_y = data->force_y[i][j][k][d] - ref->force_y[i][j][k][d];
          diff_z = data->force_z[i][j][k][d] - ref->force_z[i][j][k][d];
          has_errors |= (diff_x<-EPSILON) || (EPSILON<diff_x);
          has_errors |= (diff_y<-EPSILON) || (EPSILON<diff_y);
          has_errors |= (diff_z<-EPSILON) || (EPSILON<diff_z);
        }
      }
    }
  }

  // Return true if it's correct.
  return !has_errors;
}
