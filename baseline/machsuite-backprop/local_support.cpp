#include "xcl2.hpp"
#include <vector>
#include "backprop.h"
#include <string.h>

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON (1.0e-6)

void run_benchmark( void *vargs, std::ofstream *runtime, int iter ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  size_t vector_size_bytes_w1 = input_dimension*nodes_per_layer*sizeof(TYPE);
  size_t vector_size_bytes_w2 = nodes_per_layer*nodes_per_layer*sizeof(TYPE);
  size_t vector_size_bytes_w3 = nodes_per_layer*possible_outputs*sizeof(TYPE);
  size_t vector_size_bytes_b1 = nodes_per_layer*sizeof(TYPE);
  size_t vector_size_bytes_b2 = nodes_per_layer*sizeof(TYPE);
  size_t vector_size_bytes_b3 = possible_outputs*sizeof(TYPE);
  size_t vector_size_bytes_td = training_sets*input_dimension*sizeof(TYPE);
  size_t vector_size_bytes_tt = training_sets*possible_outputs*sizeof(TYPE);

  cl_int err;
  std::vector<TYPE,aligned_allocator<TYPE>> weights1(input_dimension*nodes_per_layer);
  std::vector<TYPE,aligned_allocator<TYPE>> weights2(nodes_per_layer*nodes_per_layer);
  std::vector<TYPE,aligned_allocator<TYPE>> weights3(nodes_per_layer*possible_outputs);
  std::vector<TYPE,aligned_allocator<TYPE>> biases1(nodes_per_layer);
  std::vector<TYPE,aligned_allocator<TYPE>> biases2(nodes_per_layer);
  std::vector<TYPE,aligned_allocator<TYPE>> biases3(possible_outputs);
  std::vector<TYPE,aligned_allocator<TYPE>> training_data(training_sets*input_dimension);
  std::vector<TYPE,aligned_allocator<TYPE>> training_targets(training_sets*possible_outputs);

  // Copy the test data
  for(int i = 0 ; i < input_dimension*nodes_per_layer ; i++){
    weights1[i] = args->weights1[i];
  }
  for(int i = 0 ; i < nodes_per_layer*nodes_per_layer ; i++){
    weights2[i] = args->weights2[i];
  }
  for(int i = 0 ; i < nodes_per_layer*possible_outputs ; i++){
    weights3[i] = args->weights3[i];
  }
  for(int i = 0 ; i < nodes_per_layer ; i++){
    biases1[i] = args->biases2[i];
    biases2[i] = args->biases2[i];
  }
  for (int i = 0; i< possible_outputs; i++) {
    biases3[i] = args->biases3[i];
  }
  for(int i = 0 ; i < training_sets*input_dimension ; i++){
    training_data[i] = args->training_data[i];
  }
  for(int i = 0 ; i < training_sets*possible_outputs ; i++){
    training_targets[i] = args->training_targets[i];
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
  std::string binaryFile = xcl::find_binary_file(device_name,"backprop");

  // import_binary_file() ia a utility API which will load the binaryFile
  // and will return Binaries.
  cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
  devices.resize(1);
  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  OCL_CHECK(err, cl::Kernel krnl_backprop(program,"backprop", &err));

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
            cl::Buffer buffer_w1(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_w1,
                                 weights1.data(),
                                 &err));
  OCL_CHECK(err,
            cl::Buffer buffer_w2(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_w2,
                                 weights2.data(),
                                 &err));
  OCL_CHECK(err,
            cl::Buffer buffer_w3(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_w3,
                                 weights3.data(),
                                 &err));

  OCL_CHECK(err,
            cl::Buffer buffer_b1(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_b1,
                                 biases1.data(),
                                 &err));

  OCL_CHECK(err,
            cl::Buffer buffer_b2(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_b2,
                                 biases2.data(),
                                 &err));
  OCL_CHECK(err,
            cl::Buffer buffer_b3(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                 vector_size_bytes_b3,
                                 biases3.data(),
                                 &err));

  OCL_CHECK(err,
            cl::Buffer buffer_td(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes_td,
                                 training_data.data(),
                                 &err));
  OCL_CHECK(err,
            cl::Buffer buffer_tt(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes_tt,
                                 training_targets.data(),
                                 &err));


  // Copy input data to device global memory d
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_w1, buffer_w2, buffer_w3, buffer_b1, buffer_b2, buffer_b3, buffer_td, buffer_tt},0/* 0 means from host*/));

  OCL_CHECK(err, err = krnl_backprop.setArg(0, buffer_w1));
  OCL_CHECK(err, err = krnl_backprop.setArg(1, buffer_w2));
  OCL_CHECK(err, err = krnl_backprop.setArg(2, buffer_w3));
  OCL_CHECK(err, err = krnl_backprop.setArg(3, buffer_b1));
  OCL_CHECK(err, err = krnl_backprop.setArg(4, buffer_w2));
  OCL_CHECK(err, err = krnl_backprop.setArg(5, buffer_b3));
  OCL_CHECK(err, err = krnl_backprop.setArg(6, buffer_td));
  OCL_CHECK(err, err = krnl_backprop.setArg(7, buffer_tt));
  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  cl::Event event;
  uint64_t nstimestart, nstimeend;
  OCL_CHECK(err, err = q.enqueueTask(krnl_backprop, NULL, &event));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_w1, buffer_w2, buffer_w3, buffer_b1, buffer_b2, buffer_b3},CL_MIGRATE_MEM_OBJECT_HOST));
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
  for(int i = 0 ; i < input_dimension*nodes_per_layer ; i++){
    args->weights1[i] =  weights1[i];
  }
  for(int i = 0 ; i < nodes_per_layer*nodes_per_layer ; i++){
    args->weights2[i] =  weights2[i];
  }
  for(int i = 0 ; i < nodes_per_layer*possible_outputs ; i++){
    args->weights3[i] =  weights3[i] ;
  }
  for(int i = 0 ; i < nodes_per_layer ; i++){
    args->biases2[i] = biases1[i];
    args->biases2[i] = biases2[i];
  }
  for (int i = 0; i< possible_outputs; i++) {
    args->biases3[i] = biases3[i];
  }
}


/* Input format:
%% Section 1
TYPE[row_size*col_size]: input matrix
%% Section 2
TYPE[f_size]: filter coefficients
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));

  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->weights1, input_dimension*nodes_per_layer);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->weights2, nodes_per_layer*nodes_per_layer);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->weights3, nodes_per_layer*possible_outputs);

  s = find_section_start(p,4);
  STAC(parse_,TYPE,_array)(s, data->biases1, nodes_per_layer);

  s = find_section_start(p,5);
  STAC(parse_,TYPE,_array)(s, data->biases2, nodes_per_layer);

  s = find_section_start(p,6);
  STAC(parse_,TYPE,_array)(s, data->biases3, possible_outputs);

  s = find_section_start(p,7);
  STAC(parse_,TYPE,_array)(s, data->training_data, training_sets*input_dimension);

  s = find_section_start(p,8);
  STAC(parse_,TYPE,_array)(s, data->training_targets, training_sets*possible_outputs);
  free(p);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights1, input_dimension*nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights2, nodes_per_layer*nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights3, nodes_per_layer*possible_outputs);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases1, nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases2, nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases3, possible_outputs);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->training_data, training_sets*input_dimension);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->training_targets, training_sets*possible_outputs);
}

/* Output format:
%% Section 1
TYPE[row_size*col_size]: solution matrix
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Zero-out everything.
  memset(vdata,0,sizeof(struct bench_args_t));
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->weights1, input_dimension*nodes_per_layer);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->weights2, nodes_per_layer*nodes_per_layer);

  s = find_section_start(p,3);
  STAC(parse_,TYPE,_array)(s, data->weights3, nodes_per_layer*possible_outputs);

  s = find_section_start(p,4);
  STAC(parse_,TYPE,_array)(s, data->biases1, nodes_per_layer);

  s = find_section_start(p,5);
  STAC(parse_,TYPE,_array)(s, data->biases2, nodes_per_layer);

  s = find_section_start(p,6);
  STAC(parse_,TYPE,_array)(s, data->biases3, possible_outputs);
  free(p);

}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights1, input_dimension*nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights2, nodes_per_layer*nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->weights3, nodes_per_layer*possible_outputs);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases1, nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases2, nodes_per_layer);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->biases3, possible_outputs);

}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i, j;
  TYPE diff;

  for(i=0; i<input_dimension; i++) {
    for(j=0; j<nodes_per_layer; j++) {
      diff = data->weights1[i*nodes_per_layer + j] - ref->weights1[i*nodes_per_layer + j];
      has_errors |= (diff<-EPSILON) || (EPSILON<diff);
    }
  }
  for(i=0; i<nodes_per_layer; i++) {
    for(j=0; j<nodes_per_layer; j++) {
      diff = data->weights2[i*nodes_per_layer + j] - ref->weights2[i*nodes_per_layer + j];
      has_errors |= (diff<-EPSILON) || (EPSILON<diff);
    }
  }
  for(i=0; i<nodes_per_layer; i++) {
    for(j=0; j<possible_outputs; j++) {
      diff = data->weights3[i*possible_outputs + j] - ref->weights3[i*possible_outputs + j];
      has_errors |= (diff<-EPSILON) || (EPSILON<diff);
    }
  }
  for(i=0; i<nodes_per_layer; i++) {
    diff = data->biases1[i] - ref->biases1[i];
    has_errors |= (diff<-EPSILON) || (EPSILON<diff);
  }
  for(i=0; i<nodes_per_layer; i++) {
    diff = data->biases2[i] - ref->biases2[i];
    has_errors |= (diff<-EPSILON) || (EPSILON<diff);
  }
  for(i=0; i<possible_outputs; i++) {
    diff = data->biases3[i] - ref->biases3[i];
    has_errors |= (diff<-EPSILON) || (EPSILON<diff);
  }
  // Return true if it's correct.
  return !has_errors;
}
