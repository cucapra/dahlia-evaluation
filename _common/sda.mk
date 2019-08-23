# Include sdaccel utilities
COMMON_REPO =  .
ABS_COMMON_REPO = $(shell readlink -n $(COMMON_REPO))

include ./utils.mk

# Include sdaccel libraries
include libs/opencl/opencl.mk
include libs/xcl2/xcl2.mk

# OCL compilation options.
MODES := hw
MODE  := $(MODES)
DEVICES   := xilinx_vcu1525_dynamic
DEVICE    := $(DEVICES)
TARGET_FREQ  := 250 # clock is set using frequency, using 4ns for testing

# The ordinary (software) C++ compiler.
CXX   := $(XILINX_SDX)/bin/xcpp
CXXFLAGS  := $(xcl2_CXXFLAGS)
LDFLAGS   := $(xcl2_LDFLAGS)
CXXFLAGS  += $(opencl_CXXFLAGS)
LDFLAGS   += $(opencl_LDFLAGS)
CXXFLAGS  += -Wall -O0 -g -std=c++14

HOST_SRCS += $(xcl2_SRCS)

# Host compiler global settings
CXXFLAGS  += -fmessage-length=0
LDFLAGS   += -lrt -lstdc++ 

# The OCL compiler.
XOCC  := $(XILINX_SDX)/bin/xocc
XOCCFLAGS := -t $(MODE) --platform $(DEVICE) --save-temps --kernel_frequency=$(TARGET_FREQ) -DSDACCEL
## How to add directives, can you add directives? other options?

# Binaries
BINARY_CONTAINERS     += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xclbin
BINARY_CONTAINER_OBJS += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xo

# emconfig stuff
XCLBIN       := ./xclbin
DSA          := $(call device2sandsa, $(DEVICE))
EMCONFIG_DIR = $(XCLBIN)/$(DSA)
CP           = cp -rf
EXECUTABLE   := sdaccel

check: all
ifeq ($(MODE),$(filter $(MODE),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
endif

.PHONY: all
all: $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

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

# Debugging targets: submit code to Buildbot as a new job
.PHONY: submit
BUILDBOT := http://ec2-54-234-195-6.compute-1.amazonaws.com:5000

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' -F make=1 -F mode=$(MODE)\
		$(BUILDBOT)/jobs

# What's this? Device check?
bar: all
	echo $(DSA)
	$(COMMON_REPO)/utility/parsexpmf.py $(DEVICE) dsa
