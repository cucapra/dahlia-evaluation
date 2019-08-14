#include "xcl2.hpp"
#include <vector> 
#include "fft.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((double)1.0e-6)

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_real = FFT_SIZE*sizeof(double);
  size_t vector_size_bytes_img = FFT_SIZE*sizeof(double);
  size_t vector_size_bytes_real_t = (FFT_SIZE/2)*sizeof(double);
  size_t vector_size_bytes_img_t = (FFT_SIZE/2)*sizeof(double);


  cl_int err;
  std::vector<double,aligned_allocator<double>> real(FFT_SIZE);
  std::vector<double,aligned_allocator<double>> img(FFT_SIZE);
  std::vector<double,aligned_allocator<double>> real_twid(FFT_SIZE/2);
  std::vector<double,aligned_allocator<double>> img_twid(FFT_SIZE/2);

  // Copy the test data 
  for(int i = 0 ; i < FFT_SIZE ; i++){
      real[i] = args->real[i];
      img[i] = args->img[i];
      if (i<FFT_SIZE/2) {
      real_twid[i] = args->real_twid[i];
      img_twid[i] = args->img_twid[i];
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
    std::string binaryFile = xcl::find_binary_file(device_name,"fft");

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_fft_stride(program,"fft", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and 
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_real(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes_real,
                                    real.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_img(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                    vector_size_bytes_img,
                                    img.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_real_t(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                       vector_size_bytes_real_t,
                                       real_twid.data(),
                                       &err));
    
    OCL_CHECK(err,
              cl::Buffer buffer_img_t(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                       vector_size_bytes_img_t,
                                       img_twid.data(),
                                       &err));
    

    // Copy input data to device global memory d
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_real, buffer_img, buffer_real_t, buffer_img_t},0/* 0 means from host*/));

    int size = 4096;
    OCL_CHECK(err, err = krnl_fft_stride.setArg(0, buffer_real));
    OCL_CHECK(err, err = krnl_fft_stride.setArg(1, buffer_img));
    OCL_CHECK(err, err = krnl_fft_stride.setArg(2, buffer_real_t));
    OCL_CHECK(err, err = krnl_fft_stride.setArg(3, buffer_img_t));

    // Launch the Kernel
    // For HLS kernels global and local size is always (1,1,1). So, it is recommended
    // to always use enqueueTask() for invoking HLS kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_fft_stride));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_real, buffer_img},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
  // OPENCL HOST CODE AREA END
  
  // Copy results 
  for(int i = 0 ; i < FFT_SIZE ; i++){
     args->real[i] =  real[i];
     args->img[i] =  img[i];
  }
}


/* Input format:
%% Section 1
double: signal (real part)
%% Section 2
double: signal (complex part)
%% Section 3
double: twiddle factor (real part)
%% Section 4
double: twiddle factor (complex part)
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_double_array(s, data->real, FFT_SIZE);

  s = find_section_start(p,2);
  parse_double_array(s, data->img, FFT_SIZE);

  s = find_section_start(p,3);
  parse_double_array(s, data->real_twid, FFT_SIZE/2);

  s = find_section_start(p,4);
  parse_double_array(s, data->img_twid, FFT_SIZE/2);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_double_array(fd, data->real, FFT_SIZE);

  write_section_header(fd);
  write_double_array(fd, data->img, FFT_SIZE);

  write_section_header(fd);
  write_double_array(fd, data->real_twid, FFT_SIZE/2);

  write_section_header(fd);
  write_double_array(fd, data->img_twid, FFT_SIZE/2);
}

/* Output format:
%% Section 1
double: freq (real part)
%% Section 2
double: freq (complex part)
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  parse_double_array(s, data->real, FFT_SIZE);

  s = find_section_start(p,2);
  parse_double_array(s, data->img, FFT_SIZE);
  free(p);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  write_double_array(fd, data->real, FFT_SIZE);

  write_section_header(fd);
  write_double_array(fd, data->img, FFT_SIZE);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  double real_diff, img_diff;

  for(i=0; i<FFT_SIZE; i++) {
    real_diff = data->real[i] - ref->real[i];
    img_diff = data->img[i] - ref->img[i];
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
