# A common Makefile for running on host with g++ and building 
# FPGA+host code with SDSoC. Specify $(KERNEL),
# $(HW_SRCS), and $(HOST_SRCS), and then `include` this file.

SOURCES   := $(HW_SRCS) $(HOST_SRCS)
OBJECTS   := $(SOURCES:%.cpp=%.o)
DEPENDS   := $(SOURCES:%.cpp=%.d)

HWLIST    := $(KERNEL) $(HW_SRCS)
# HLS Directives(hardware only option)
ifeq ($(DIRECTIVE),1)
HWLIST    += -hls-tcl $(HWDIR)
endif

SDSXX     := sds++ -sds-hw $(HWLIST) -sds-end
SDSFLAGS  := -sds-pf zed -clkid 3 -poll-mode 1 -verbose
CXX       := g++
CXXFLAGS  := -Wall -O3

# SDSoC estimation mode(hardware only option)
ifeq ($(ESTIMATE),1)
SDSFLAGS  += -perf-est-hw-only
PERFLAGS  := -F estimate=1
endif

GENERATED := _sds .Xil sd_card $(KERNEL).bit
COMPILER  := $(SDSXX) $(SDSFLAGS)

# Software mode
ifeq ($(SOFTWARE),1)
CXXFLAGS  := -Wall -O3 -Wno-unused-label
GENERATED := output.data
COMPILER  := $(CXX)
endif

$(KERNEL): $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(COMPILER) $(CXXFLAGS) -c $< -o $@

.PHONY: submit run clean

# Hardware only command at present to submit job to Buildbot
submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(PERFLAGS) -F make=1 http://gorgonzola.cs.cornell.edu:8000/jobs


# Software only command to run executable
run: $(KERNEL) input.data check.data
	./$(KERNEL) input.data check.data

clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(KERNEL) $(GENERATED)

# Use the compiler's -MM flag to generate header dependencies. (sds++ seems to
# not work correctly for this.)
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
include $(DEPENDS)
