#include "xcl2.hpp"
#include <vector>
#include "fft.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes = 512*sizeof(TYPE);

  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> work_x(512);
  std::vector<TYPE,aligned_allocator<TYPE>> work_y(512);
  // Copy the test data
  for(int i = 0 ; i < 512 ; i++){
      work_x[i] = args->work_x[i];
      work_y[i] = args->work_y[i];
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
    std::string binaryFile = xcl::find_binary_file(device_name,"fft");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_fft_transpose(program,"fft", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_workx(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes,
                                    work_x.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_worky(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes,
                                    work_y.data(),
                                    &err));


    // Copy input data to device global memory d
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_workx, buffer_worky},0/* 0 means from host*/));

    OCL_CHECK(err, err = krnl_fft_transpose.setArg(0, buffer_workx));
    OCL_CHECK(err, err = krnl_fft_transpose.setArg(1, buffer_worky));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    cl::Event event;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err, err = q.enqueueTask(krnl_fft_transpose, NULL, &event));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_workx, buffer_worky},CL_MIGRATE_MEM_OBJECT_HOST));
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
  for(int i = 0 ; i < 512 ; i++){
     args->work_x[i] =  work_x[i];
     args->work_y[i] =  work_y[i];
  }
}

/* Input format:
%% Section 1
TYPE[512]: signal (real part)
%% Section 2
TYPE[512]: signal (complex part)
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->work_x, 512);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->work_y, 512);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->work_x, 512);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->work_y, 512);
}

/* Output format:
%% Section 1
TYPE[512]: freq (real part)
%% Section 2
TYPE[512]: freq (complex part)
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->work_x, 512);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->work_y, 512);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->work_x, 512);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->work_y, 512);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  double real_diff, img_diff;

  for(i=0; i<512; i++) {
    real_diff = data->work_x[i] - ref->work_x[i];
    img_diff = data->work_y[i] - ref->work_y[i];
    has_errors |= (real_diff<-EPSILON) || (EPSILON<real_diff);
    //if( has_errors )
      //printf("%d (real): %f (%f)\n", i, real_diff, EPSILON);
    has_errors |= (img_diff<-EPSILON) || (EPSILON<img_diff);
    //if( has_errors )
      //printf("%d (img): %f (%f)\n", i, img_diff, EPSILON);
  }

  // Return true if it's correct.
  return !has_errors;
}
