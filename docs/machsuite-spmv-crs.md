# ORIGINAL
Resides in original/spmv/crs

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Add tripcount to `spmv_2` for estimation.
max: 10, min: 0, total: 1666, number: 494, avg: 3
BRAM resource directives commented out as they break the flow. #35
Data copy pragma used for `val` to avoid #34.

# REWRITE
For loop changed to a while loop as bounds are determined at runtime.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
