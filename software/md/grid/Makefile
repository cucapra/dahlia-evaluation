# Set kernel name
KERNEL           = md
ALGORITHM_NAME   = grid
VERSION_NAME	 = software

# Set host source and headers
HOST_SRC         = ../../common/harness.c
SW_SRCS          = local_support.c ../../common/support.c
SW_H             = ../../common/support.h

# Set kernel files
KERNEL_SRC = $(KERNEL).c
KERNEL_H   = $(KERNEL).h

SRCS  = $(SW_SRCS) $(KERNEL_SRC)
ALL_SRCS = $(SRCS) $(HOST_SRC)
FILES = $(ALL_SRCS) $(SW_H) $(KERNEL_H)

#-------------------------
# Leave the rest to harness
#-------------------------
include ../../common/harness.mk

echo:
	@echo ${KERNEL}
	
