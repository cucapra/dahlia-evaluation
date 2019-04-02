# A common Makefile for building FPGA+host code with SDSoC. Specify $(KERNEL),
# $(HW_SRCS), and $(HOST_SRCS), and then `include` this file.

SOURCES := $(HW_SRCS) $(HOST_SRCS)

OBJECTS := $(SOURCES:%.cpp=%.o)
DEPENDS := $(SOURCES:%.cpp=%.d)

SDSXX := sds++ -sds-hw $(KERNEL) $(HW_SRCS) -sds-end
SDSFLAGS := -sds-pf zed -clkid 3 -poll-mode 1 -verbose
CXX := g++
CXXFLAGS := -Wall -O3

# SDSoC estimation mode.
ifdef ESTIMATE
SDSFLAGS += -perf-est-hw-only
endif

$(KERNEL): $(OBJECTS)
	$(SDSXX) $(SDSFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(SDSXX) $(SDSFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(KERNEL) _sds

# Use the compiler's -MM flag to generate header dependencies. (sds++ seems to
# not work correctly for this.)
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

# Include the generated dependencies.
include $(DEPENDS)
