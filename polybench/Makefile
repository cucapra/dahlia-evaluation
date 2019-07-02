.PHONY: clean all

SOURCES:=$(wildcard *.fuse)
OBJS:=$(patsubst %.fuse,%.cpp,$(SOURCES))

all: $(OBJS)

%.cpp: %.fuse.pre
	fuse $< -o $@

%.fuse.pre: %.fuse
	m4 $< > $@


clean:
	rm *.cpp
