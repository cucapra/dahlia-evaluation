.PHONY: clean depend

KERNEL:=stencil_3d
HW_SRCS:=$(KERNEL).cpp
HOST_SRCS:=main.cpp local_support.cpp common/support.cpp

SOURCES:= $(HW_SRCS) $(HOST_SRCS)

OBJECTS:=$(SOURCES:%.cpp=%.o)
DEPENDS:=$(SOURCES:%.cpp=%.d)

SDSXX:=sds++ -sds-pf zed -sds-hw $(KERNEL) $(HW_SRCS) -sds-end -clkid 3 -poll-mode 1 -verbose
CXX:=g++
CXXFLAGS:=-Wall -O3

$(KERNEL): $(OBJECTS)
	$(SDSXX) $(SDSFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(SDSXX) $(SDSFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(DEPENDS) $(KERNEL)

# Use the compiler's -MM flag to generate header dependencies. SDSXX seems to
# not work correctly for this.
%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

print-%  : ; @echo $* = $($*)

# Include the generated dependencies.
include $(DEPENDS)
