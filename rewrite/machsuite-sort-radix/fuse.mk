# By default, we look for the Fuse compiler in a directory adjacent to the
# benchmarks directory.
FUSE := ../../../seashell/target/scala-2.12/fuse.jar

%.cpp: %.fuse
	$(FUSE) $^ > $@

.PHONY: fuse
fuse: $(HW_SRCS)
