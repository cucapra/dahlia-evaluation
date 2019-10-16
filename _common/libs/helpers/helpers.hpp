#include "xcl2.hpp"
#include <CL/cl2.hpp>

namespace helpers {
  cl::Kernel get_kernel(cl::Context &context, cl::Device device, std::string kernel_name, cl_int &err);
}
