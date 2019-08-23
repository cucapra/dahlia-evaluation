# A common Makefile for synthesizing benchmark kernels only with Vivado tools 
# or to do functional testing by building host and hardware sources with 
# standard software C++ compiler and running with data files for debugging. 
# Benchmarks should `include` this file after defining at least these variables:
#
# - KERNEL: The name of the hardware function. (This is also used as the name
#   of the generated executable in software builds.)
# - HW_SRCS: The C++ source files containing the hardware function.
# 
# For software runs,
# - HOST_SRCS: The C++ source files containing software code (and a call to
#   the hardware function).
# - DATA: Input files that the executable will need for execution.
#
# These variables may also be useful:
#
# - DIRECTIVES: Provide a set of optimizations for Vivado tools to implement.
# - ESTIMATE: Use Vivado HLS, but do not synthesize: only provides HLS RTL.

SOURCES   := $(HW_SRCS) $(HOST_SRCS)
OBJECTS   := $(SOURCES:%.cpp=%.o)
DEPENDS   := $(SOURCES:%.cpp=%.d)

# Vivado compilation options.
PLATFORM  := xc7z020clg484-1 # to stop Buildbot changing platform
CLOCK		  := 7 # we use 7ns to test

# The software executable name. We use the kernel name by default.
TARGET    ?= $(KERNEL)
GENERATED := output.data

# Define flags for Buildbot.
CURLFLAGS := -F hwname=$(KERNEL)
CURLFLAGS += -F directives=$(DIRECTIVES)
CURLFLAGS += -F skipexec=1
CURLFLAGS += -F platform=$(PLATFORM)

ifeq ($(ESTIMATE),1)
VHLS_MODE := hls
CURLFLAGS += -F estimate=1
else
VHLS_MODE := syn
endif
GENERATED += benchmark.prj vivado_hls.log run_hls.tcl

.PHONY: vhls
vhls: run_hls.tcl
	vivado_hls -f $<
	! grep 'Command failed' vivado_hls.log

ifeq ($(DIRECTIVES),)
DIRARG := -
else
DIRARG := $(DIRECTIVES)
endif
run_hls.tcl:
	python gen_vhls_tcl.py $(VHLS_MODE) $(DIRARG) $(KERNEL) $(PLATFORM) $(CLOCK) $(HW_SRCS) > $@

.PHONY: submit
BUILDBOT := http://gorgonzola.cs.cornell.edu:8000

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(CURLFLAGS) -F make=1 \
		$(BUILDBOT)/jobs

# The ordinary (software) C++ compiler.
CXX       := g++
CXXFLAGS  := -Wall -O3

# Compile source files.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the program.
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -Wno-unused-label $(LDFLAGS) $^ -o $@

# Use the compiler's -MM flag to generate header dependencies.
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
include $(DEPENDS)

.PHONY: sw run
sw: $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(TARGET) $(GENERATED)

