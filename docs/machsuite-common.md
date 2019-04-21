This keeps track of changes to files harness.c, support.h and support.c in the original benchmarks

## Changes made to the common files

# SOFTWARE
All `.c` files are changed to `.cpp`
In harness, char * data types are used in the initial c version. These are specificed constant or casted for cpp.
Simple performance measurement is added in `harness.cpp`. To print the results `inttypes.h` is included.
Relevant functions are added to `support.h` with `time.h` library included.

# BASELINE
data malloc is changed to sds_alloc
`sds_lib.h` is included in `support.h`.

# REWRITE

# OPTIMIZED BASELINE

# OPTIMIZED REWRITE
