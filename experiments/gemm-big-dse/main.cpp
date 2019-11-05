#include <stdlib.h>
#include <ostream>
#include <fstream>
#include <assert.h>
#include <random>
#include <ap_int.h>
#include "xcl2.hpp"
#include "helpers.hpp"

#define TYPE ap_int<32>
#define N 4096

using std::default_random_engine;
using std::uniform_int_distribution;

int gen_random() {
  static default_random_engine e;
  static uniform_int_distribution<int> dist(0, 100);

  return dist(e);
}

void golden_dot(TYPE *m1, TYPE *m2, TYPE *prod) {
  TYPE sum;
  for(int i=0; i < N; i++) {
    prod[0] += m1[i] * m2[i];
  }
}

int check_golden(TYPE *hw_comp, TYPE *golden) {

  for (auto l = 0; l < N; l++) {
    if (hw_comp[l] != golden[l]) {
      std::cout << "Error: Values at index " << l << " differ." << std::endl;
      std::cout << "hw: " << hw_comp[l] << " gold: " << golden[l] << std::endl;
      return 1;
    }
  }
  std::cout << "Success." << std::endl;
  return 0;
}

void fill_random(TYPE *m) {
  for (auto l = 0; l < N; l++) {
    m[l] = (TYPE)gen_random();
  }
}

int main(int argc, char **argv) {

  // Buffer to store the output
  std::vector<TYPE, aligned_allocator<TYPE>> m1(N);
  std::vector<TYPE, aligned_allocator<TYPE>> m2(N);
  std::vector<TYPE, aligned_allocator<TYPE>> prod(1);
  std::vector<TYPE, aligned_allocator<TYPE>> prod_golden(1);

  cl_int err;
  auto device = xcl::get_xil_devices()[0];

  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err,
      cl::CommandQueue cmd_queue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

  cl::Kernel krnl_gemm = helpers::get_kernel(context, device, "dot", err);

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
      cl::Buffer m1_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        sizeof(TYPE) * N,
        m1.data(),
        &err));

  OCL_CHECK(err,
      cl::Buffer m2_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        sizeof(TYPE) * N,
        m2.data(),
        &err));

  OCL_CHECK(err,
      cl::Buffer prod_buffer(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        sizeof(TYPE) * 1,
        prod.data(),
        &err));

  OCL_CHECK(err, err = krnl_gemm.setArg(0, m1_buffer));
  OCL_CHECK(err, err = krnl_gemm.setArg(1, m2_buffer));
  OCL_CHECK(err, err = krnl_gemm.setArg(2, prod_buffer));

  int check = 0;

  // Runtime log file.
  std::ofstream runtime ("runtime.log");
  if (!runtime.is_open()) {
    std::cout << "Error: Failed to open output file.";
    return 1;
  }

  runtime << "iteration" << "," << "time(ms)" << std::endl;

  for (auto i = 0; i < 10; i++) {
    // Random values for input matrices.
    fill_random(m1.data());
    fill_random(m2.data());

    cl::Event event;
    uint64_t nstimestart, nstimeend;

    // Copy input data
    OCL_CHECK(err,
        err = cmd_queue.enqueueMigrateMemObjects(
          { m1_buffer, m2_buffer }, 0 /* 0 means from host */));

    // Launch the Kernel
    OCL_CHECK(err, err = cmd_queue.enqueueTask(krnl_gemm, NULL, &event));

    OCL_CHECK(err,
        err = cmd_queue.enqueueMigrateMemObjects(
          { prod_buffer }, CL_MIGRATE_MEM_OBJECT_HOST));

    cmd_queue.finish();

    OCL_CHECK(err,
        err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,
          &nstimestart));
    OCL_CHECK(err,
        err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,
          &nstimeend));

    auto t = nstimeend - nstimestart;
    std::cout << "Iteration " << i << ": " << t/1000000.0 << " ms." << std::endl;
    runtime << i << "," << t/1000000.0 << std::endl;

    // Golden computation
    golden_dot(m1.data(), m2.data(), prod_golden.data());

    if (prod[0] != prod_golden[0]) {
      std::cout << "Results differ. Golden: " << prod_golden[0]
                << " Computed: " << prod[0];
      break;
    }

  }

  runtime.close();

  return check;

}
