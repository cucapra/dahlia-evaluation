KERNEL           = stencil
HW_SRCS          = $(KERNEL).cpp
HOST_SRCS        = main.cpp local_support.cpp support.cpp bench.cpp
DIRECTIVES       = $(KERNEL)_dir
DATA             = input.data check.data

include vhls.mk
-include fuse.mk
