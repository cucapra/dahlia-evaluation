 #Points to Utility Directory
COMMON_REPO =  .
ABS_COMMON_REPO = $(shell readlink -n $(COMMON_REPO))

include ./utils.mk

MODES := hw
MODE := $(MODES)
DEVICES := xilinx_vcu1525_dynamic
DEVICE := $(DEVICES)
XCLBIN := ./xclbin
DSA := $(call device2sandsa, $(DEVICE))

CXX := $(XILINX_SDX)/bin/xcpp
XOCC := $(XILINX_SDX)/bin/xocc

#Include Libraries
include libs/opencl/opencl.mk
include libs/xcl2/xcl2.mk
CXXFLAGS += $(xcl2_CXXFLAGS)
LDFLAGS += $(xcl2_LDFLAGS)
HOST_SRCS += $(xcl2_SRCS)
CXXFLAGS += $(opencl_CXXFLAGS) -Wall -O0 -g -std=c++14
LDFLAGS += $(opencl_LDFLAGS)

HOST_SRCS += $(EXECUTABLE).cpp

# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 

# Kernel compiler global settings
CLFLAGS += -t $(MODE) --platform $(DEVICE) --save-temps 


EXECUTABLE = host

EMCONFIG_DIR = $(XCLBIN)/$(DSA)

BINARY_CONTAINERS += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xclbin
BINARY_CONTAINER_OBJS += $(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xo

CP = cp -rf


check: all
ifeq ($(MODE),$(filter $(MODE),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
endif

.PHONY: all clean cleanall docs emconfig
all: $(EXECUTABLE) $(BINARY_CONTAINERS) emconfig

.PHONY: exe
exe: $(EXECUTABLE)

# Building kernel
$(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xo: $(KERNEL).cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k $(KERNEL) -I'$(<D)' -o'$@' '$<'
$(XCLBIN)/$(KERNEL).$(MODE).$(DSA).xclbin: $(BINARY_CONTAINER_OBJS)
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -l $(LDCLFLAGS) --nk $(KERNEL):1 -o'$@' $(+)

# Building Host
$(EXECUTABLE): $(HOST_SRCS) $(HOST_HDRS)
	mkdir -p $(XCLBIN)
	$(CXX) $(CXXFLAGS) $(HOST_SRCS) $(HOST_HDRS) -o '$@' $(LDFLAGS)


emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)



# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)/{*sw_emu*,*hw_emu*} 
	-$(RMDIR) sdaccel_* TempConfig system_estimate.xtxt *.rpt
	-$(RMDIR) ./*.ll _xocc_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) $(XCLBIN)
	-$(RMDIR) ./_x

checkhost: all 
	sudo sh -c 'source /opt/xilinx/xrt/setup.sh ; ./$(EXECUTABLE)'



.PHONY: submit run
BUILDBOT := http://ec2-54-234-195-6.compute-1.amazonaws.com:5000

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' -F make=1 -F mode=$(MODE)\
		$(BUILDBOT)/jobs

bar: all
	echo $(DSA)
	$(COMMON_REPO)/utility/parsexpmf.py $(DEVICE) dsa