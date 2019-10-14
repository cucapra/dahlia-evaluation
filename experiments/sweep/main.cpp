#include <stdlib.h>
#include <assert.h>
#include <random>
#include "xcl2.hpp"
#include "gemm.h"

using std::default_random_engine;
using std::uniform_int_distribution;

int gen_random() {
    static default_random_engine e;
    static uniform_int_distribution<int> dist(0, 100);

    return dist(e);
}

void golden_gemm(TYPE *m1, TYPE *m2, TYPE *prod) {
	TYPE sum;
	for(int i=0; i < ROW_SIZE; i++) {
		for(int j=0; j < COL_SIZE; j++) {
			sum = 0;
			for(int k=0; k<ROW_SIZE; k++) {
				sum += m1[i * COL_SIZE + k] * m2[k * COL_SIZE + j];
			}
			prod[i * COL_SIZE + j] = sum;
		}
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
	std::vector<TYPE, aligned_allocator<TYPE>> prod(N);
	std::vector<TYPE, aligned_allocator<TYPE>> prod_golden(N);

  // Random values for input matrices.
  fill_random(m1.data());
  fill_random(m2.data());

  // Golden computation
  golden_gemm(m1.data(), m2.data(), prod_golden.data());

	cl_int err;
	auto devices = xcl::get_xil_devices();
	auto device = devices[0];

	OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
	OCL_CHECK(err,
			cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
	OCL_CHECK(err,
			std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

	// find_binary_file() is a utility API which will search the xclbin file for
	// targeted mode (sw_emu/hw_emu/hw) and for targeted platforms.
	std::string binaryFile = xcl::find_binary_file(device_name, "gemm");

	// import_binary_file() ia a utility API which will load the binaryFile
	// and will return Binaries.
	auto bins = xcl::import_binary_file(binaryFile);
	devices.resize(1);
	OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
	OCL_CHECK(err, cl::Kernel krnl_gemm(program, "gemm", &err));

	// Allocate Buffer in Global Memory
	// Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
	// Device-to-host communication
	OCL_CHECK(err,
			cl::Buffer m1_buffer(context,
				CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
				sizeof(ap_int<32>) * N,
				m1.data(),
				&err));

	OCL_CHECK(err,
			cl::Buffer m2_buffer(context,
				CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
				sizeof(ap_int<32>) * N,
				m2.data(),
				&err));

	OCL_CHECK(err,
			cl::Buffer prod_buffer(context,
				CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
				sizeof(ap_int<32>) * N,
				prod.data(),
				&err));

	OCL_CHECK(err, err = krnl_gemm.setArg(0, m1_buffer));
	OCL_CHECK(err, err = krnl_gemm.setArg(1, m2_buffer));
	OCL_CHECK(err, err = krnl_gemm.setArg(2, prod_buffer));

	// Launch the Kernel
	// For HLS kernels global and local size is always (1,1,1). So, it is recommended
	// to always use enqueueTask() for invoking HLS kernel
	OCL_CHECK(err, err = q.enqueueTask(krnl_gemm));

	OCL_CHECK(err,
			err = q.enqueueMigrateMemObjects(
				{ prod_buffer }, CL_MIGRATE_MEM_OBJECT_HOST));

	q.finish();

  return check_golden(prod.data(), prod_golden.data());

}
