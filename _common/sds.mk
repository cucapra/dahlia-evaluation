# A common Makefile for running on host with g++ and building
# FPGA+host code with SDSoC. Specify $(KERNEL),
# $(HW_SRCS), and $(HOST_SRCS), and then `include` this file.

SOURCES   := $(HW_SRCS) $(HOST_SRCS)
OBJECTS   := $(SOURCES:%.cpp=%.o)
DEPENDS   := $(SOURCES:%.cpp=%.d)

# SDSoC compilation options.
PLATFORM  := zed
CLOCK_ID  := 3

# The final executable name. We use the kernel name by default, but this can
# be overridden.
TARGET    ?= $(KERNEL)

# The ordinary (software) C++ compiler.
CXX       := g++
CXXFLAGS  := -Wall -O3

# Build the list of hardware kernels to compile, for SDSoC. When DIRECTIVE is
# supplied, include the path to search for TCL directive files.
HWLIST    := $(KERNEL) $(HW_SRCS)
ifeq ($(DIRECTIVE),1)
HWLIST    += -hls-tcl $(HWDIR)
endif

# The SDSoC compiler.
SDSXX     := sds++
SDSFLAGS  := -sds-hw $(HWLIST) -sds-end -sds-pf $(PLATFORM) \
	-clkid $(CLOCK_ID) -poll-mode 1 -verbose

# In estimation mode, SDSoC only emits resource metrics and does not actually
# produce a bitstream.
ifeq ($(ESTIMATE),1)
SDSFLAGS  += -perf-est-hw-only
CURLFLAGS := -F estimate=1
endif

# Set COMPILER (and a few other things) according to whether we're doing a
# software or hardware run.
ifeq ($(SOFTWARE),1)
COMPILER  := $(CXX)
CXXFLAGS  += -Wno-unused-label
GENERATED := output.data
else
COMPILER  := $(SDSXX) $(SDSFLAGS)
GENERATED := _sds .Xil sd_card $(TARGET).bit
endif

# Copy data files to executable directory for hardware
ifeq ($(or $(ESTIMATE),$(SOFTWARE)),0)
sd_card: $(TARGET)
	for d in $(DATA); do cp $$d sd_card/; done
endif

# Link the program.
$(TARGET): $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Compile source files.
%.o: %.cpp
	$(COMPILER) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(TARGET) $(GENERATED)

# Use the compiler's -MM flag to generate header dependencies. (sds++ seems to
# not work correctly for this.)
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
include $(DEPENDS)


# Debugging targets: submit code to Buildbot as a new job, and execute
# the compiled (software) executable.

.PHONY: submit run
BUILDBOT := http://gorgonzola.cs.cornell.edu:8000

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(CURLFLAGS) -F make=1 \
		$(BUILDBOT)/jobs

run: $(TARGET)
	./$(TARGET) 

