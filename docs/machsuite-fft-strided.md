# ORIGINAL
Resides in original/fft/strided

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for both arguments.
Add tripcount to inner loop for estimation.

# REWRITE
For loops turned to while loops, outer due to complex pattern, inner due to starting point determined dynamically. Operation order adjusted and temporary variables introduced to reduce dependency.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
