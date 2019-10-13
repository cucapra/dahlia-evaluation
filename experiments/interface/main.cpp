#include <stdlib.h>
#include <assert.h>
#include "xcl2.hpp"
#include <ap_int.h>

int main(int argc, char **argv) {

  // Buffer to store the output
  std::vector<ap_int<32>, aligned_allocator<ap_int<32>>> out(1);
  std::vector<ap_int<32>, aligned_allocator<ap_int<32>>> A(1024);
  out[0] = 0;

  for (int i = 0; i < 1024; i++) {
    A[i] = 1;
  }

  // Golden computation
  ap_int<32> check = 0;
  for (int i = 0; i < 1024; i++) {
    check += A[i];
  }
  // END

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
  std::string binaryFile = xcl::find_binary_file(device_name, "partition");

  // import_binary_file() ia a utility API which will load the binaryFile
  // and will return Binaries.
  auto bins = xcl::import_binary_file(binaryFile);
  devices.resize(1);
  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  OCL_CHECK(err, cl::Kernel krnl_partition(program, "partition", &err));

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err,
      cl::Buffer buffer_A(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        sizeof(ap_int<32>) * 1024,
        A.data(),
        &err));

  OCL_CHECK(err,
      cl::Buffer buffer_out(context,
        CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
        sizeof(ap_int<32>),
        out.data(),
        &err));

  OCL_CHECK(err, err = krnl_partition.setArg(0, buffer_A));
  OCL_CHECK(err, err = krnl_partition.setArg(1, buffer_out));

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is recommended
  // to always use enqueueTask() for invoking HLS kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_partition));

  OCL_CHECK(err,
      err = q.enqueueMigrateMemObjects(
        { buffer_out }, CL_MIGRATE_MEM_OBJECT_HOST));

  q.finish();

  // Writes to memory are not observed till .finish() is called.
  std::cout << "completed. out: " << out[0] << " res: " << check << std::endl;

  if (out[0] != check) {
    return 1;
  }

  return 0;

}
