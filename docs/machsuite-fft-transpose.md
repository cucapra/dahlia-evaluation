# ORIGINAL
Resides in original/fft/transpose

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `fft1D_512` to `fft` in all three source files.

# BASELINE
Data zero copy pragma used for both arguments.

# REWRITE
Inlining defines.
Define PHI and other values in source file.
Use explicit double casting.
Using an inline fft8 function because we don't have pass by pointer for non-array variables.
Using arrays of size 8 rather than unrolling the loop manually.
Using access pattern arrays to use for loops of size 8.
We don't have a function return. So manually assigining external functions values.
Manually change PHI value which is rounded off in our code gen.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
