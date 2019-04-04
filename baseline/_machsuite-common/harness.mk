# A common Makefile for building FPGA+host code with SDSoC. Specify $(KERNEL),
# $(HW_SRCS), and $(HOST_SRCS), and then `include` this file.

SOURCES  := $(HW_SRCS) $(HOST_SRCS)

OBJECTS  := $(SOURCES:%.cpp=%.o)
DEPENDS  := $(SOURCES:%.cpp=%.d)

HWLIST   := $(KERNEL) $(HW_SRCS)
# HLS Directives
ifdef DIRECTIVE
HWLIST   += -hls-tcl $(HWDIR)
endif

SDSXX    := sds++ -sds-hw $(HWLIST) -sds-end
SDXFLAGS := -sds-pf zed -clkid 3 -poll-mode 1 -verbose
CXX      := g++
CXXFLAGS := -Wall -O3

# SDSoC estimation mode.
ifdef ESTIMATE
SDXFLAGS += -perf-est-hw-only
PERFLAGS := -F estimate=1
endif

$(KERNEL): $(OBJECTS)
	$(SDSXX) $(SDXFLAGS) $(CXXFLAGS) $^ -o $@
#	$(SDSXX) $(SDSFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(SDSXX) $(SDXFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: submit clean

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(PERFLAGS) -F make=1 http://gorgonzola.cs.cornell.edu:8000/jobs

clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(KERNEL) _sds

# Use the compiler's -MM flag to generate header dependencies. (sds++ seems to
# not work correctly for this.)
#%.d: %.cpp
#	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
#include $(DEPENDS)
