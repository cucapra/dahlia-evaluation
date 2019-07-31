# ORIGINAL
Resides in original/stencil/stencil3d

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `stencil3d` to `stencil` in all three source files.

# BASELINE
Data copy pragma used for `C` to avoid #34.
Outer loop pipeline removed to meet timing on Zed board.

# REWRITE
3D arrays are used. Accumulation order is changed.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
