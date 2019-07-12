# By default, we look for the Fuse compiler in a directory adjacent to the
# benchmarks directory.
FUSE := ../../../seashell/target/scala-2.12/fuse.jar

# The `make fuse` target rebuilds the HLS C++ program from the Fuse source.
# For now, it assumes that HW_SRCS consists of a single *.cpp file.
# This rule does not use typical Make prerequisite management to avoid
# rebuilding the C++ source, even when the Fuse file is newer---we want this
# to be an exclusively manual process.
.PHONY: fuse
fuse:
	$(FUSE) -n $(KERNEL) $(HW_SRCS:%.cpp=%.fuse) -o $(HW_SRCS)
