# ORIGINAL
Resides in original/gemm/blocked

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `bbgemm` to `gemm` in all three source files.

# BASELINE
Data copy pragma used for `m1` to avoid #34.

# REWRITE
2D arrays are used. Tile loop iterator is stepping by 1 and multiplied by tile factor at computation because our language doesn't support it, which makes this version use more multipliers. Loop order changed to allow combine.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
