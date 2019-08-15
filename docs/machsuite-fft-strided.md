# ORIGINAL
Resides in original/fft/strided

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for both arguments.
Add tripcount to inner loop for estimation.
max: 512, min: 0, total: 5120, number: 10, avg: 512
Directives file is stale, uses non existent labels and even the argument numbers differ. Best effort to update it.

# REWRITE
For loops turned to while loops, outer due to complex pattern, inner due to starting point determined dynamically. Operation order adjusted and temporary variables introduced to reduce dependency.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
