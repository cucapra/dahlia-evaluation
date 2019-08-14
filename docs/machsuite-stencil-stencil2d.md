# ORIGINAL
Resides in original/stencil/stencil2d

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data copy pragma used for `orig` to avoid #34.

# REWRITE
2D arrays are used. `temp` variable is converted to two `temp` variables for each loop over filter dimension.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
Use rotational views. Increase array size to partition.
## DSE

### Current best
Unroll k1 and k2 - EKNUVM3Odpc

