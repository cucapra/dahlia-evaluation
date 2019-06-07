# ORIGINAL
Resides in original/spmv/ellpack

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ellpack` to `spmv` in all three source files.

# BASELINE
Data zero copy pragma used for `out`.
Data copy pragma used for `nzval` to avoid #34.

# REWRITE
All `bit<32>`s are manually changed to `int`s.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
