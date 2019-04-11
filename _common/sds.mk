# A common Makefile for building FPGA+host code with SDSoC. Specify $(KERNEL),
# $(HW_SRCS), and $(HOST_SRCS), and then `include` this file.

SOURCES   := $(HW_SRCS) $(HOST_SRCS)

# Software mode.
ifdef SOFTWARE
OBJECTS   := $(SOURCES:%.c=%.o)
DEPENDS   := $(SOURCES:%.c=%.d)

CXX       := $(CC)
CXXFLAGS  := -Wall -O3 -Wno-unused-label
GENERATED := output.data
COMPILER  := $(CXX)
EXT       := c
else
OBJECTS   := $(SOURCES:%.cpp=%.o)
DEPENDS   := $(SOURCES:%.cpp=%.d)

HWLIST    := $(KERNEL) $(HW_SRCS)
# HLS Directives
ifdef DIRECTIVE
HWLIST    += -hls-tcl $(HWDIR)
endif

SDSXX     := sds++ -sds-hw $(HWLIST) -sds-end
SDSFLAGS  := -sds-pf zed -clkid 3 -poll-mode 1 -verbose
CXX       := g++
CXXFLAGS  := -Wall -O3

# SDSoC estimation mode.
ifdef ESTIMATE
SDSFLAGS  += -perf-est-hw-only
PERFLAGS  := -F estimate=1
endif

GENERATED := _sds .Xil sd_card $(KERNEL).bit
COMPILER  := $(SDSXX) $(SDSFLAGS)
EXT       := cpp
endif

$(KERNEL): $(OBJECTS)
	$(COMPILER) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.$(EXT)
	$(COMPILER) $(CXXFLAGS) -c $< -o $@

.PHONY: submit run clean

# Hardware only command at present, to submit job to Buildbot
submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(PERFLAGS) -F make=1 http://gorgonzola.cs.cornell.edu:8000/jobs

# Software only command to run executable
run: $(KERNEL) input.data check.data
	./$(KERNEL) input.data check.data

clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(KERNEL) $(GENERATED)

# Use the compiler's -MM flag to generate header dependencies. (sds++ seems to
# not work correctly for this.)
%.d: %.$(EXT)
	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
include $(DEPENDS)
