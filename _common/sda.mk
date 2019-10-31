# Include sdaccel utilities
COMMON_REPO =  .
ABS_COMMON_REPO = $(shell readlink -n $(COMMON_REPO))

include ./utils.mk

# Include sdaccel libraries
include libs/opencl/opencl.mk
include libs/xcl2/xcl2.mk

# OCL compilation options.
DEVICE    := xilinx_vcu1525_dynamic
TARGET_FREQ  := 250 # clock is set using frequency, using 4ns for testing

# Check if we have a valid MODE set
ifeq (,$(filter $(MODE),sw_emu hw_emu hw estimate))
$(error "Invalid MODE: $(MODE). Must be one of: hw, hw_emu, sw_emu, estimate")
endif

# The ordinary (software) C++ compiler.
CXX   := $(XILINX_SDX)/bin/xcpp
CXXFLAGS  := $(xcl2_CXXFLAGS)
LDFLAGS   := $(xcl2_LDFLAGS)
CXXFLAGS  += $(opencl_CXXFLAGS)
LDFLAGS   += $(opencl_LDFLAGS)

HOST_SRCS += $(xcl2_SRCS)

# Add helper library
HOST_SRCS += ${COMMON_REPO}/libs/helpers/helpers.cpp
CXXFLAGS += -I${COMMON_REPO}/libs/helpers

# Compiler flags
CXXFLAGS  += -Wall -O0 -g -std=c++14

# Host compiler global settings
CXXFLAGS  += -fmessage-length=0
LDFLAGS   += -lrt -lstdc++

# The OCL compiler.
XOCC  := $(XILINX_SDX)/bin/xocc
# If in estimation mode, add the required flags
ifeq ($(MODE),estimate)
XOCCFLAGS += --report estimate
override MODE := hw_emu
endif

XOCCFLAGS += -t $(MODE) --platform $(DEVICE) --save-temps --kernel_frequency=$(TARGET_FREQ) -DSDACCEL
## How to add directives, can you add directives? other options?


# Binaries
BINARY_CONTAINERS     += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xclbin
BINARY_CONTAINER_OBJS += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xo

# emconfig stuff
XCLBIN       := ./xclbin
DSA          := $(call device2sandsa, $(DEVICE))
EMCONFIG_DIR = $(XCLBIN)/$(DSA)
CP           = cp -rf
EXECUTABLE   := exe

check: all
ifeq ($(MODE),$(filter $(MODE),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
endif

.PHONY: all
all: $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

# emconfig
.PHONY: emconfig
emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

# Building kernel
$(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xo: $(HW_SRCS)
	mkdir -p $(XCLBIN)
	$(XOCC) $(XOCCFLAGS) -c -k $(KERNEL) -I'$(<D)' -o'$@' '$<'
$(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xclbin: $(BINARY_CONTAINER_OBJS)
	mkdir -p $(XCLBIN)
	$(XOCC) $(XOCCFLAGS) -l $(LDCLFLAGS) --nk $(KERNEL):1 -o'$@' $(+)

# Building Host
$(EXECUTABLE): $(HOST_SRCS) $(HOST_HDRS)
	mkdir -p $(XCLBIN)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)

# Check for host
checkhost: all
	sudo sh -c 'source /opt/xilinx/xrt/setup.sh ; ./$(EXECUTABLE)'

# Cleaning stuff
.PHONY: clean cleanall
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*}
	-$(RMDIR) sdaccel_* TempConfig system_estimate.xtxt *.rpt
	-$(RMDIR) ./*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) $(XCLBIN)
	-$(RMDIR) ./_x


# Print value of a Makefile variable.
print-%:  ; @echo $* = $($*)
