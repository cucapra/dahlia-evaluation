#include "helpers.hpp"

namespace helpers {
  // Get the kernel corresponding to the `kernel_name` for `device`.
  cl::Kernel get_kernel(
      cl::Context &context,
      cl::Device device,
      std::string kernel_name,
      cl_int &err) {

    OCL_CHECK(err,
        std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    // Find binary for the xclbin file for current mode.
    std::string binaryFile = xcl::find_binary_file(device_name, kernel_name);

    // import_binary_file() ia a utility API which will load the binaryFile
    // and will return Binaries.
    auto bins = xcl::import_binary_file(binaryFile);
    OCL_CHECK(err, cl::Program program(context, { device }, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl(program, kernel_name.c_str(), &err));

    return krnl;
  }
}
