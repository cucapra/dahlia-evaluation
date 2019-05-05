%.cpp: %.fuse.pre
	fuse $< -o $@

%.fuse.pre: %.fuse
	m4 $< > $@

.PHONY: clean

clean:
	rm *.cpp
